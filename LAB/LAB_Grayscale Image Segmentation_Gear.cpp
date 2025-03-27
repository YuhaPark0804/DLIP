/*------------------------------------------------------/
* Image Proccessing with Deep Learning
* OpenCV  : LAB: Grayscale Image Segmentation - Gear
* Created : 2025-03-21
* Modified: 2025-03-27
* Name    : Yuha Park
------------------------------------------------------*/


#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void processGearImage(Mat& src, Mat& detectedTeeth, Mat& gearWithoutArea, Mat& damagedTeeth, int gearIndex);
void displayGearImages(const Mat& src, const Mat& detectedTeeth, const Mat& gearWithoutArea, const Mat& damagedTeeth, int index);

int main() {
    vector<string> imagePaths = {
        "../../Image/Lab_GrayScale_Gears/Gear1.jpg",
        "../../Image/Lab_GrayScale_Gears/Gear2.jpg",
        "../../Image/Lab_GrayScale_Gears/Gear3.jpg",
        "../../Image/Lab_GrayScale_Gears/Gear4.jpg"
    };

    for (size_t i = 0; i < imagePaths.size(); i++) {
        Mat src = imread(imagePaths[i], IMREAD_COLOR); // Load image in color mode
        Mat detectedTeeth, gearWithoutArea, damagedTeeth;

        processGearImage(src, detectedTeeth, gearWithoutArea, damagedTeeth, (int)i);
        displayGearImages(src, detectedTeeth, gearWithoutArea, damagedTeeth, (int)i);

    }
    waitKey(0);
    return 0;
}

void processGearImage(Mat& src, Mat& detectedTeeth, Mat& gearWithoutArea, Mat& damagedTeeth, int gearIndex) {
    if (src.empty()) {
        cout << "File Read Failed" << endl;
        return;
    }

    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY); // Convert BGR to GRAY scale for findContours
    Mat blurred;
    GaussianBlur(gray, blurred, Size(5, 5), 2, 2);  // Use Gaussian blur to reduce noise


    vector<vector<Point>> contours;
    findContours(blurred, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    // src: Must be a grayscale image
    // RETR_EXTERNAL: Finds external contours
    // CHAIN_APPROX_SIMPLE: Stores contour points efficiently

    float maxRadius = 0.0;
    Point2f gearCenter;

    for (size_t i = 0; i < contours.size(); i++) {
        Point2f center;
        float radius;

        // minEnclosingCircle(): Finds a circle of the minimum area enclosing a 2D point set.
        // Select the largest radius circle to set 1. Gear center coordinates 2. Radius
        minEnclosingCircle(contours[i], center, radius);
        if (radius > maxRadius) {         
            maxRadius = radius;
            gearCenter = center;
        }
    }

    // Find the minimum tooth gap radius from the gear center
    // Create a circular mask covering the gear center
    float minRadius = maxRadius;
    for (size_t i = 0; i < contours.size(); i++) {
        for (size_t j = 0; j < contours[i].size(); j++) {
            float distance = norm(gearCenter - Point2f(contours[i][j]));
            if (distance < minRadius) {
                minRadius = distance;
            }
        }
    }

    // Gear number
    cout << "\n\nGear[" << gearIndex + 1 << "]" << endl;

    // Diameter of the gear (Root diameter)
    cout << "Diameter of the gear (Root diameter): " << minRadius << endl;

    // Create Mask
    Mat mask = Mat::zeros(src.size(), CV_8UC1); // CV_8UC1: 1-channel 8-bit unsigned integer (Grayscale image)
    circle(mask, gearCenter, (int)minRadius, Scalar(255), -1);  // -1: Filled circle

    // Apply mask to original src image
    Mat result = src.clone();
    result.setTo(Scalar(0, 0, 0), mask);


    // Use opening operation to separate the gear teeth
    Mat element = getStructuringElement(MORPH_RECT, Size(6, 6)); // structure element: shape, size
    Mat dst;
    erode(result, dst, element);
    dilate(dst, dst, element);
  

    Mat teethEdges;
    Canny(dst, teethEdges, 100, 200); // canny(): edge detection
    gearWithoutArea = dst.clone();

    // findContours(): Find contours of the gear teeth
    vector<vector<Point>> teethContours;
    findContours(teethEdges, teethContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    detectedTeeth = dst.clone(); // Image for detected teeth area calculation
    damagedTeeth = src.clone();  // Image to highlight damaged teeth with yellow circles


    /***************************Average Teeth Contours Calculate************************/
    double avgArea = 0.0 ;
    for (size_t j = 0; j < teethContours.size(); j++) {
        avgArea += contourArea(teethContours[j]);       // Calculate average area of gear teeth
    }
    avgArea /= teethContours.size();
    cout << "avg. Teeth Area: " << avgArea << endl;
    /***************************************END***************************************/

    cout << "Teeth numbers: " << teethContours.size() << endl;

    // Number of defective teeth
    int defTeeth = 0;

    for (size_t i = 0; i < teethContours.size(); i++) { //teethContours.size() = 20
        double area = contourArea(teethContours[i]); // Calculate contour area

        Moments m = moments(teethContours[i]); // Compute moments of the contour
        Point contourCenter(m.m10 / m.m00, m.m01 / m.m00); // Compute contour center

        Point textPosition; // Store coordinates to position text
        // Adjust position 0.3 towards gear center
        textPosition.x = contourCenter.x + (gearCenter.x - contourCenter.x) * 0.3;
        textPosition.y = contourCenter.y + (gearCenter.y - contourCenter.y) * 0.3;

        // Offset to the left due to overall rightward bias
        float offset = 20.0;
        textPosition.x -= offset;

        // Set defect criteria: minimum area is 0.9x average, maximum is 1.2x average
        double minArea = avgArea * 0.9;
        double maxArea = avgArea * 1.2;


        // Text to be displayed
        string areaText = to_string(static_cast<int>(area));

        
        // Color coding detectedTeeth based on area
        if (area > minArea && area < maxArea) { // Normal teeth in green
            drawContours(detectedTeeth, teethContours, (int)i, Scalar(0, 255, 0), 2);
            drawContours(gearWithoutArea, teethContours, (int)i, Scalar(0, 255, 0), 2);
            putText(detectedTeeth, areaText, textPosition, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1.5);
        }
        else { // Defective teeth in red
            drawContours(detectedTeeth, teethContours, (int)i, Scalar(0, 0, 255), 2);
            drawContours(gearWithoutArea, teethContours, (int)i, Scalar(0, 0, 255), 2);
            putText(detectedTeeth, areaText, textPosition, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1.5);

            // damagedTeeth Image: Highlight defective teeth with yellow circles
            int radius = 30;
            circle(damagedTeeth, Point(contourCenter.x, contourCenter.y), radius, Scalar(0, 255, 255), 2, LINE_8);
        
            defTeeth++;
        }
    }
    
    string Quality = "Fail";
    if (defTeeth == 0) Quality = "Pass";

    cout << "Number of defevtive teeth: " << defTeeth << endl;
    cout << "Quality Inspection (Pass or Fail): " << Quality << "\n\n" << endl;
}

void displayGearImages(const Mat& src, const Mat& detectedTeeth, const Mat& gearWithoutArea, const Mat& damagedTeeth, int index) {
   
    int windowXOffset = 0;
    int windowYOffset = 250;
    int yOffset = windowYOffset * index;

    string srcWindowName = "Gear " + to_string(index + 1) + " Original";
    namedWindow(srcWindowName, WINDOW_AUTOSIZE);
    imshow(srcWindowName, src);  // Src Image

    string detectedTeethWindowName = "Detected Teeth " + to_string(index + 1);
    namedWindow(detectedTeethWindowName, WINDOW_AUTOSIZE);
    imshow(detectedTeethWindowName, detectedTeeth);  // Detected tooth with area displayed

    string gearWithoutAreaWindowName = "Gear Without Area " + to_string(index + 1);
    namedWindow(gearWithoutAreaWindowName, WINDOW_AUTOSIZE);
    imshow(gearWithoutAreaWindowName, gearWithoutArea);  // Detected tooth without area displayed

    string damagedTeethWindowName = "Damaged Teeth " + to_string(index + 1);
    namedWindow(damagedTeethWindowName, WINDOW_AUTOSIZE);
    imshow(damagedTeethWindowName, damagedTeeth);  // Image of damaged teeth in yellow circle

    moveWindow(srcWindowName, windowXOffset, windowYOffset * index);
    moveWindow(detectedTeethWindowName, windowXOffset + 460, windowYOffset * index);
    moveWindow(gearWithoutAreaWindowName, windowXOffset + 920, windowYOffset * index);
    moveWindow(damagedTeethWindowName, windowXOffset + 1380, windowYOffset * index);
}



