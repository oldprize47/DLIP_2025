/* ------------------------------------------------------ /
*Image Proccessing with Deep Learning
* OpenCV : Filter Demo
* Created : 2021 - Spring
------------------------------------------------------ */

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;
void main()
{
	cv::Mat src, dst;
	src = cv::imread("../../Image/blurry_moon.tif", 0);

	int i = 3;
	Size kernelSize = cv::Size(i, i);

	namedWindow("Origin", WINDOW_AUTOSIZE); // WINDOW_NORMAL
	imshow("Origin", src);

	/* Blur */
	cv::blur(src, dst, cv::Size(i, i), cv::Point(-1, -1));
	namedWindow("Blur", WINDOW_AUTOSIZE); // WINDOW_NORMAL
	imshow("Blur", dst);

	/* Gaussian Filter */
	cv::GaussianBlur(src, dst, kernelSize, 0, 0);
	namedWindow("Gaussian", WINDOW_AUTOSIZE);
	imshow("Gaussian", dst);

	/* Median Filter */
	cv::medianBlur(src, dst, i);
	namedWindow("Median", WINDOW_AUTOSIZE);
	imshow("Median", dst);


	/* Laplacian Filter */
	int kernel_size = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	cv::Laplacian(src, dst, ddepth, kernel_size, scale, delta, cv::BORDER_DEFAULT);
	// Conver src(8bits unsign) -> 16bits sign - for arithmetic
	src.convertTo(src, CV_16S);
	// subtraction
	cv::Mat result_laplcaian = src - dst;
	// recover, re-convert
	result_laplcaian.convertTo(result_laplcaian, CV_8U);
	
	namedWindow("Laplacian", WINDOW_AUTOSIZE);
	cv::imshow("Laplacian", result_laplcaian);


	/* 2D Convolution of a filter kernel */
	/* Design a normalized box filter kernel 5 by 5 */
	src.convertTo(src, CV_8UC1);
	Mat kernel;
	kernel = (cv::Mat_<float>(3, 3) <<
		-1, -1, -1,
		-1, 8, -1,
		-1, -1, -1);
	delta = 0.0;
	ddepth = -1;
	kernel_size = 5;
	Point anchor = Point(-1, -1);
	cv::filter2D(src, dst, ddepth, kernel, anchor,delta);

	namedWindow("Conv2D", WINDOW_AUTOSIZE);
	cv::imshow("Conv2D", src + dst);

	cv::waitKey(0);
}