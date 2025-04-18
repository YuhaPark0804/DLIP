/*------------------------------------------------------/
* Image Proccessing with Deep Learning
* OpenCV  : LAB: Color Image Segmentation - Magic Cloak
* Created : 2025-04-11
* Modified: 2025-04-17
* Name    : Yuha Park
------------------------------------------------------*/

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat image;
Point origin;
Rect selection;
bool selectObject = false;
bool trackObject = false;
int hmin = 1, hmax = 179, smin = 130, smax = 255, vmin = 160, vmax = 255;

/// On mouse event 
static void onMouse(int event, int x, int y, int, void*);

int main()
{
	// VideoCapture cap(0); // open the video camera no. 0

	string box_image = "../../LAB/LAB_Magic_Cloak/RED.jpg";
	image = imread(box_image);

	//if (!cap.isOpened())  // if not success, exit program
	//{
	//	cout << "Cannot open the video cam" << endl;
	//	return -1;
	//}

	//bool bSuccess = cap.read(image); // read a new frame from video
	//if (!bSuccess) //if not success, break loop
	//{
	//	cout << "Cannot read a frame from video stream" << endl;
	//	return -1;
	//}


	// TrackBar 설정
	namedWindow("Source", 0);
	setMouseCallback("Source", onMouse, 0);
	createTrackbar("Hmin", "Source", &hmin, 179, 0);
	createTrackbar("Hmax", "Source", &hmax, 179, 0);
	createTrackbar("Smin", "Source", &smin, 255, 0);
	createTrackbar("Smax", "Source", &smax, 255, 0);
	createTrackbar("Vmin", "Source", &vmin, 255, 0);
	createTrackbar("Vmax", "Source", &vmax, 255, 0);

	Mat image_disp, hsv, hue, mask, dst;
	Mat dst_track = Mat::zeros(image.size(), CV_8UC3);

	vector<vector<Point> > contours;

	while (true)
	{

		// cap.read(image);

		image.copyTo(image_disp);

		imshow("Source", image);
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
		morphologyEx(dst, dst, MORPH_CLOSE, Mat(), Point(-1, -1), 1);


		namedWindow("InRange", 0);

		imshow("InRange", dst);

		/// once mouse has selected an area bigger than 0
		if (trackObject)
		{
			trackObject = false;					// Terminate the next Analysis loop
			Mat roi_HSV(hsv, selection); 			// Set ROI by the selection box		
			Scalar means, stddev;
			meanStdDev(roi_HSV, means, stddev);
			cout << "\n Selected ROI Means= " << means << " \n stddev= " << stddev;

			// Change the value in the trackbar according to Mean and STD //
			hmin = MAX((means[0] - stddev[0]), 0);
			hmax = MIN((means[0] + stddev[0]), 179);
			setTrackbarPos("Hmin", "Source", hmin);
			setTrackbarPos("Hmax", "Source", hmax);

			/******** Repeat for S trackbar ********/
			smin = MAX((means[1] - stddev[1]), 0);
			smax = MIN((means[1] + stddev[1]), 255);
			setTrackbarPos("Smin", "Source", smin);
			setTrackbarPos("Smax", "Source", smax);

			/******** Repeat for V trackbar ********/
			vmin = MAX((means[2] - stddev[2]), 0);
			vmax = MIN((means[2] + stddev[2]), 255);
			setTrackbarPos("Vmin", "Source", vmin);
			setTrackbarPos("Vmax", "Source", vmax);

		}


		if (selectObject && selection.area() > 0)  // Left Mouse is being clicked and dragged
		{
			// Mouse Drag을 화면에 보여주기 위함
			Mat roi_RGB(image_disp, selection);
			bitwise_not(roi_RGB, roi_RGB);
			// imshow("Source", image_disp);
		}


		///  Find All Contour   ///
		findContours(dst, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);


		if (contours.size() > 0)
		{
			/// Find the Contour with the largest area ///
			double maxArea = 0;
			int maxArea_idx = 0;

			for (int i = 0; i < contours.size(); i++)
				if (contourArea(contours[i]) > maxArea) {
					maxArea = contourArea(contours[i]);
					maxArea_idx = i;
				}

			///  Draw the max Contour on Black-background  Image ///
			Mat dst_out = Mat::zeros(dst.size(), CV_8UC3);
			drawContours(dst_out, contours, maxArea_idx, Scalar(0, 0, 255), 2, 8);
			// namedWindow("Contour", 0);
			// imshow("Contour", dst_out);


			/// Draw the Contour Box on Original Image ///
			Rect boxPoint = boundingRect(contours[maxArea_idx]);
			rectangle(image_disp, boxPoint, Scalar(255, 0, 255), 3);
			// namedWindow("Contour_Box", 0);
			// imshow("Contour_Box", image_disp);


			/// Continue Drawing the Contour Box  ///
			dst_track -= Scalar(10, 0, 10);
			rectangle(dst_track, boxPoint, Scalar(255, 0, 255), 3);
			// namedWindow("Contour_Track", 0);
			// imshow("Contour_Track", dst_track);

		}


		char c = (char)waitKey(10);
		if (c == 27)
			break;
	} // end of for(;;)

	return 0;

}



/// On mouse event 
static void onMouse(int event, int x, int y, int, void*)
{
	if (selectObject)  // for any mouse motion
	{
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = abs(x - origin.x) + 1;
		selection.height = abs(y - origin.y) + 1;
		selection &= Rect(0, 0, image.cols, image.rows);  /// Bitwise AND  check selectin is within the image coordinate
	}

	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		selectObject = true;
		origin = Point(x, y);
		break;
	case EVENT_LBUTTONUP:
		selectObject = false;
		if (selection.area())
			trackObject = true;
		break;
	}

}