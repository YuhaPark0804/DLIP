#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>


using namespace cv;
using namespace std;

/// Global variables

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int Ratio = 3;
int kernel_size = 3;
const char* window_name = "Edge Map";


/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*)
{
	/// Reduce noise with a kernel 3x3   
	/// src_gray --> detected_edges
	// 
		
	GaussianBlur(src_gray, detected_edges, Size(kernel_size, kernel_size), 2, 2);

	vector<Vec3f> circles;

	HoughCircles(detected_edges, circles, HOUGH_GRADIENT, 2, detected_edges.rows / 2.0, 200, 50, 200, 220);

	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);

		/* draw the circle center */
		circle(src, center, 3, Scalar(0, 255, 0), -1, LINE_8, 0);

		/* draw the circle outline */
		circle(src, center, radius, Scalar(0, 0, 255), 3, LINE_8, 0);
	}

	/// Canny detector
	///  detected_edges-->detected_edges:  lowThreshold, lowThreshold*ratio, kernel_size
	//

	// Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * Ratio, kernel_size);


	/// Using Canny's output as a mask, we display our result
	dst = Scalar::all(0);

	// src.clone();
	src.copyTo(dst, detected_edges);
	imshow(window_name, dst);
}


/** @function main */
int main(int argc, char** argv)
{
	/// Load an image

	const char* filename = "../../Image/EdgeLineImages/TrafficSign1.png";


	/// Read the image
	src = imread(filename, 1);

	if (!src.data)
	{
		return -1;
	}


	/// Create a matrix of the same type and size as src (for dst)
	dst.create(src.size(), src.type());

	/// Convert the image to grayscale
	cvtColor(src, src_gray, COLOR_BGR2GRAY);

	/// Create a window
	namedWindow(window_name, WINDOW_AUTOSIZE);

	/// Create a Trackbar for user to enter threshold
	createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

	/// Show the image
	CannyThreshold(0, 0);

	/// Wait until user exit program by pressing a key
	waitKey(0);

	return 0;
}