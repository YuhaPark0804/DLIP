/*------------------------------------------------------------------------------------------\
@ Deep Learning & Image Processing  by Young-Keun Kim - Handong Global University

Author          : Yuha Park
Created         : 03-07-2025
Modified        : 03-07-2025
Language/ver	: C++
Course		: Deep Learning & Image Processing

Description     : "DLIP_Assignment_OpenCV_Basics"
/------------------------------------------------------------------------------------------*/

#include <iostream>
#include <opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	/*  read image  */
	String filename1 = "../../Image/HGU_logo.jpg";			// class
	Mat img = imread(filename1);							//Mat class
	Mat img_gray = imread("../../Image/HGU_logo.jpg", 0);   // read in grayscale

	/*  write image  */
	String filename2 = "writeTest.jpg";		// C++ class/syntax (String, cout, cin)
	imwrite(filename2, img);

	/*  display image  */
	
	//namedWindow("image", WINDOW_AUTOSIZE);
	//imshow("image", img);

	namedWindow("image_gray", WINDOW_AUTOSIZE);
	imshow("image_gray", img_gray);

	/*  Resize  */
	Mat resizedImg;
	resize(img_gray, resizedImg, Size(img.rows/2, img.cols/2)); // 464 x 480 px
	//imshow("resizedImg", resizedImg);


	/*  Crop  */
	Rect r(135, 140, 180, 200);	 // (x, y, width, height)
	Mat roiImg = img_gray(r);
	imshow("roiImg", roiImg);

	/*  Flip  */
	Mat dsp;
	flip(resizedImg, dsp, 0);
	//imshow("fliping", dsp);

	/*  create empty image  */ 
	// ouput #1
	Mat EmptyImage1 = Mat::ones(img.size(), CV_8UC1) * 255;

	for (int i = 0; i < resizedImg.rows; i++) {
		for (int j = 0; j < resizedImg.cols; j++) {
			EmptyImage1.at<uchar>(i, j) = resizedImg.at<uchar>(i, j);
		}
	}
	imshow("EmptyImage1", EmptyImage1);

	// ouput #2
	Mat EmptyImage2 = Mat::ones(img.size(), CV_8UC1) * 255;

	int X_output2 = (img.rows / 2) - (resizedImg.rows / 2);
	int Y_output2 = (img.cols / 2) - (resizedImg.cols / 2);

	for (int i = X_output2; i < resizedImg.rows + X_output2; i++) {
		for (int j = Y_output2; j < resizedImg.cols + Y_output2; j++) {
			EmptyImage2.at<uchar>(i, j) = dsp.at<uchar>(i - X_output2, j - Y_output2);
		}
	}
	imshow("EmptyImage2", EmptyImage2);

	// ouput #3
	Mat EmptyImage3 = Mat::ones(img.size(), CV_8UC1) * 255;

	int X_output3 = img.rows / 2;
	int Y_output3 = img.cols / 2;
	
	for (int i = X_output3; i < roiImg.rows + X_output3; i++) {
		for (int j = Y_output3; j < roiImg.cols + Y_output3; j++) {
			EmptyImage3.at<uchar>(i, j) = roiImg.at<uchar>(i - X_output3, j - Y_output3);
		}
	}
	imshow("EmptyImage3", EmptyImage3);
	


	waitKey(0);
}
