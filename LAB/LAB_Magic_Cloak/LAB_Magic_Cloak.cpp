//#include "opencv2/video/tracking.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include <ctype.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat image, init_image;
Point origin;
Rect selection;
bool selectObject = false;
bool trackObject = false;
// int hmin = 1, hmax = 100, smin = 115, smax = 182, vmin = 161, vmax = 197;
int hmin = 1, hmax = 126, smin = 93, smax = 255, vmin = 111, vmax = 255; // box
// int hmin = 44, hmax = 179, smin = 19, smax = 255, vmin = 161, vmax = 255; // ipad
// int hmin = 95, hmax = 179, smin = 106, smax = 255, vmin = 0, vmax = 255; // red car

int main()
{
	VideoCapture cap("LAB_MagicCloak_Sample1.mp4"); // open the video camera no. 0
	VideoCapture sea_video("sea.mp4");
	
	// VideoCapture cap(0);

	/*string box_image = "background.jpg";
	init_image = imread(box_image);*/


	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the video mp4" << endl;
		return -1;
	}

	if (!sea_video.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the sea video mp4" << endl;
		return -1;
	}

	// cap >> init_image;

	bool bSuccess = cap.read(image); // read a new frame from video
	if (!bSuccess) //if not success, break loop
	{
		cout << "Cannot read a frame from video stream" << endl;
		return -1;
	}

	bool bSuccess2 = sea_video.read(image); // read a new frame from video
	if (!bSuccess2) //if not success, break loop
	{
		cout << "Cannot read a frame from video stream" << endl;
		return -1;
	}


	Mat image_disp, hsv, hue, mask, dst;
	// Mat dst_track = Mat::zeros(image.size(), CV_8UC3);

	vector<vector<Point> > contours;

	while (true)
	{
		cap >> image;
		sea_video >> init_image;

		image.copyTo(image_disp);

		if (image.empty()) {
			printf("empty image");
			return 0;
		}
		if (init_image.empty()) {
			printf("empty init_image");
			return 0;
		}
		
		imshow("Source", image);

		resize(init_image, init_image, image.size());

		/******** Convert BGR to HSV ********/
		// input mat: image
		// output mat: hsv
		cvtColor(image, hsv, COLOR_BGR2HSV); //// convert RGB/BGR to HSV (hue saturation value) with H range 0..180 if 8 bit image, color conversions

		/******** Add Pre-Processing such as filtering etc  ********/
		int kernal_size = 5;
		GaussianBlur(hsv, hsv, Size(kernal_size, kernal_size), 2, 2);
				

		/// set dst as the output of InRange
		inRange(hsv, Scalar(MIN(hmin, hmax), MIN(smin, smax), MIN(vmin, vmax)),
			Scalar(MAX(hmin, hmax), MAX(smin, smax), MAX(vmin, vmax)), dst);


		/******** Add Post-Processing such as morphology etc  ********/

		morphologyEx(dst, dst, MORPH_OPEN, Mat(), Point(-1, -1), 1);
		// morphologyEx(dst, dst, MORPH_CLOSE, Mat(), Point(-1, -1), 1);
		dilate(dst, dst, Mat(), Point(-1, -1), 2);

		namedWindow("InRange", 0);
		imshow("InRange", dst); // binery image

		// bitwise_and()
		Mat and_dst, and_dst2, fin_dst, xor_dst, not_dst;

		// and_dst = dst & init_image;
		bitwise_and(init_image, init_image, and_dst, dst);
		bitwise_not(dst, not_dst);

		bitwise_and(image, image, and_dst2, not_dst);


		// imshow("and_dst2", and_dst2);
		fin_dst = and_dst + and_dst2;
		imshow("fin_dst", fin_dst);


		char c = (char)waitKey(10);
		if (c == 27)
			break;
	} // end of for(;;)

	return 0;
}
