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

	namedWindow("MyVideo", WINDOW_NORMAL);

	int key = 0;
	int kernel_size = 11;
	int filter_type = 0;
	Mat result_laplcaian;
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
			filter_type = 2;	// Laplacian(3x3)
		}
		else if (key == 'g' || key == 'G')
		{
			filter_type = 3;	// Laplacian(3x3)
		}
		else if (key == 'm' || key == 'M') {
			filter_type = 4;
		}
		else if (key == 'n' || key == 'N') {
			filter_type = 0;
		}

		if (key == 'u' || key == 'U') {
			kernel_size += 2;
		}
		else if ((key == 'd' || key == 'D') && kernel_size > 1) {
			kernel_size -= 2;
		}
			
		if (filter_type == 1)
			blur(src, dst, cv::Size(kernel_size, kernel_size), cv::Point(-1, -1));	
		else if (filter_type == 2) {
			Laplacian(src, dst, CV_16S, kernel_size, 1, 0, cv::BORDER_DEFAULT);
			src.convertTo(src, CV_16S);
			dst = src - dst;
			dst.convertTo(dst, CV_8U);
			src.convertTo(src, CV_8UC1);
		}else if (filter_type == 3)
			GaussianBlur(src, dst, cv::Size(kernel_size, kernel_size), 0, 0);
		else if (filter_type == 4)
			medianBlur(src, dst, kernel_size);
		else
			src.copyTo(dst);

		imshow("MyVideo", dst);
	}
	return 0;
}
