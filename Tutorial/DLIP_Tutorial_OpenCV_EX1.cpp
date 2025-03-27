#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	/*  read src  */
	Mat img = imread("../../Image/testImage.jpg");
	imshow("img", img);

	/* Flip src image */
	// Add code here and show image
	Mat dst;
	flip(img, dst, 0);
	imshow("dst", dst);

	/*  Crop(Region of Interest)  original image */
	// Add code here and show image


	/*  Show source(src) and destination(dst)  */
	// Add code here
	waitKey(0);
}