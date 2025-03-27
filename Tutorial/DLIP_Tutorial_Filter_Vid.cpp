/*------------------------------------------------------/
* Image Proccessing with Deep Learning
* OpenCV : Filter Demo - Video
* Created: 2021-Spring
------------------------------------------------------*/

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
	/*  open the video camera no.0  */
	VideoCapture cap(0);

	if (!cap.isOpened())	// if not success, exit the programm
	{
		cout << "Cannot open the video cam\n";
		return -1;
	}

	namedWindow("MyVideo", WINDOW_AUTOSIZE);

	int key = 0;
	int kernel_size = 11;
	int filter_type = 0;

	/* Laplacian Filter */
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;


	while (1)
	{
		Mat src, dst;

		/*  read a new frame from video  */
		bool bSuccess = cap.read(src);

		if (!bSuccess)	// if not success, break loop
		{
			cout << "Cannot find a frame from  video stream\n";
			break;
		}


		key = waitKey(30);
		int input = 0;
		
		if (key == 27) // wait for 'ESC' press for 30ms. If 'ESC' is pressed, break loop
		{
			cout << "ESC key is pressed by user\n";
			break;
		}
		else if (key == 'b' || key == 'B')
		{
			filter_type = 1;	// blur
		}
		else if (key == 'l' || key == 'L')
		{
			filter_type = 2;	// Laplacian
		}
		else if (key == 'm' || key == 'M')
		{
			filter_type = 3;	// Median
		}
		else if (key == 'g' || key == 'G')
		{
			filter_type = 4;	// Gaussian
		}
		/* Keyboard input for other filters*/
		else if (key == 'u' || key == 'U') {
			kernel_size += 2;
		}
		else if (key == 'd' || key == 'D') {
			if (kernel_size < 3) kernel_size = 3;
			else				 kernel_size -= 2;
		}		

		if (filter_type == 1)
			blur(src, dst, cv::Size(kernel_size, kernel_size), cv::Point(-1, -1));
		else if (filter_type == 2)
		{
			cv::Laplacian(src, dst, ddepth, kernel_size, scale, delta, cv::BORDER_DEFAULT);
			src.convertTo(src, CV_16S);
			cv::Mat result_laplcaian = src - dst;
			result_laplcaian.convertTo(result_laplcaian, CV_8U);
			dst = result_laplcaian;
		}
		else if (filter_type == 3)
		{
			cv::medianBlur(src, dst, kernel_size);
		}
		else if (filter_type == 4)
		{
			cv::GaussianBlur(src, dst, cv::Size(kernel_size, kernel_size), 0);
		}
		else {
			src.copyTo(dst);
		}
		/* Apply filter accroding to filter type */
			
		imshow("MyVideo", dst);
	}
	return 0;
}