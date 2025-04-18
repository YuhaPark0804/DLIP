/*------------------------------------------------------/
* Image Proccessing with Deep Learning
* OpenCV  : TU: Edge_Line_Circle_Detection
* Created : 2025-04-01
* Modified: 2025-04-04
* Name    : Yuha Park
------------------------------------------------------*/

#include "opencv2/imgcodecs.hpp"  
#include "opencv2/highgui.hpp"    
#include "opencv2/imgproc.hpp"    
#include "iostream"               

using namespace cv;               // Use the OpenCV namespace to simplify code
using namespace std;              // Use the standard namespace for console output functions

// Function prototype for lane detection and display
void detectAndDisplayLanes(const string& filename);

int main(int argc, char** argv)
{
    // Vector containing file paths of input images
    vector<string> filenames = {
        "../../Image/EdgeLineImages/Lane_center.JPG",   // Path to the first image
        "../../Image/EdgeLineImages/Lane_changing.JPG"  // Path to the second image
    };


    for (const auto& file : filenames) {
        detectAndDisplayLanes(file);  // Call the lane detection function for each image
    }

    waitKey();
    return 0;
}

void detectAndDisplayLanes(const string& filename) {

    Mat src = imread(filename, IMREAD_GRAYSCALE);
    Mat src_color = imread(filename, IMREAD_COLOR);

    // Declare matrices to hold processed images
    Mat dst, cdst, cdstP, masked_dst;

    // Check if the image is loaded successfully
    if (src.empty()) {
        printf("Error opening image\n");
        return;
    }

    // Create a mask with the same size as the source image, initialized to black (all zeros)
    Mat mask = Mat::zeros(src.size(), CV_8UC1);

    // Define a region of interest (ROI) by setting the lower third of the mask to white (255)
    rectangle(mask, Point(0, src.rows * 2 / 3), Point(src.cols, src.rows), Scalar(255), FILLED);

    // Perform Canny edge detection
    Canny(src, dst, 50, 200, 3);

    // Apply the mask to the edge-detected image using bitwise AND operation
    bitwise_and(dst, dst, masked_dst, mask);

    // Convert the edge-detected image to BGR color space for visualization
    cvtColor(dst, cdst, COLOR_GRAY2BGR);
    cdstP = cdst.clone();

    // Vector to store detected lines
    vector<Vec4i> lines;
    // Apply Probabilistic Hough Line Transform to detect lines in the masked edge image
    HoughLinesP(masked_dst, lines, 2, CV_PI / 180, 40, 20, 10);

    // Vectors to store positive and negative slope lines and their parameters
    vector<Vec4i> posLines, negLines;
    vector<float> posSlopes, negSlopes;
    vector<float> posY, negY;

    // Iterate over the detected lines
    for (size_t i = 0; i < lines.size(); i++) {
        Vec4i l = lines[i];

        // Calculate the slope of the line
        double slope = static_cast<double>(l[3] - l[1]) / (l[2] - l[0]);

        // Filter lines based on slope to identify lane lines
        if (abs(slope) > 0.46 && abs(slope) < 2.26) {
            // Draw the line on the image
            line(cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);

            // Calculate the y-intercept of the line
            float y_intercept = -l[0] * slope + l[1];

            // Store the first positive slope line
            if (slope > 0 && posLines.empty()) {
                posLines.push_back(l);
                posSlopes.push_back(slope);
                posY.push_back(y_intercept);
            }
            // Store the first negative slope line
            else if (slope < 0 && negLines.empty()) {
                negLines.push_back(l);
                negSlopes.push_back(slope);
                negY.push_back(y_intercept);
            }
        }
    }

    // Calculate the intersection point (vanishing point) of the two lane lines
    float x_point = (negY[0] - posY[0]) / (posSlopes[0] - negSlopes[0]);
    float y_point = posSlopes[0] * x_point + posY[0];

    // Define the intersection point
    Point point_intersect = Point(x_point, y_point);

    // Draw a white circle at the intersection point on the color image
    circle(src_color, point_intersect, 5, Scalar(255, 255, 255), 2, LINE_8, 0);

    // Define the bottom of the image
    float y_EndOfImg = cdstP.rows - 1;
    // Calculate the x-coordinates where the lane lines intersect the bottom of the image
    float x_extension_0 = (y_EndOfImg - posY[0]) / posSlopes[0];
    float x_extension_1 = (y_EndOfImg - negY[0]) / negSlopes[0];

    // Draw the right lane line in green
    line(src_color, Point(x_extension_0, y_EndOfImg), Point(x_point, y_point), Scalar(0, 255, 0), 2, LINE_AA);

    // Draw the left lane line in red
    line(src_color, Point(x_extension_1, y_EndOfImg), Point(x_point, y_point), Scalar(0, 0, 255), 2, LINE_AA);

    // Draw a vertical line from the intersection point downward in blue
    line(src_color, Point(x_point, y_point), Point(x_point, y_EndOfImg), Scalar(255, 0, 0), 1, LINE_AA);

    // Vector to store the three points defining the triangular region
    vector<Point> ThreePoint;
    ThreePoint.push_back(Point(x_extension_0, y_EndOfImg));
    ThreePoint.push_back(Point(x_extension_1, y_EndOfImg));
    ThreePoint.push_back(Point(x_point, y_point));

    // Create a copy of the color image for overlay
    Mat overlay = src_color.clone();
    fillConvexPoly(overlay, ThreePoint, Scalar(0, 0, 255));

    double alpha = 0.3; // Transparency (0: completely transparent, 1: completely opaque)
    addWeighted(overlay, alpha, src_color, 1 - alpha, 0, src_color);

    // Show results
    imshow(string("Lane Detection - ") + filename, src_color);
}