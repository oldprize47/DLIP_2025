/*------------------------------------------------------------------------------------------\
@ Deep Learning & Image Processing  by Young-Keun Kim - Handong Global University

Author          : Sangheon Park
Created         : 01-04-2019
Modified        : 2025-03-07
Language/ver	: C++
Course		: Deep Learning & Image Processing

Description     : "Assignment1"
/------------------------------------------------------------------------------------------*/

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	// Load the image
	Mat src = imread("../../Image/HGU_logo.jpg", 0);

	if (src.empty())
	{
		cout << "Error: Couldn't open the image." << endl;
		return -1;
	}

	// Initialize dst with the same size as srcGray
	Mat window = Mat::zeros(src.size(), src.type());
	window = 255;

	/*	make image	*/
	Mat resizedImg, flipImg, cropImg;
	resize(src, resizedImg, Size(src.cols / 2, src.rows / 2));
	flip(resizedImg, flipImg, 1);
	
	int rect_size = 198;
	Rect r(src.rows / 2 - rect_size/2 - 12, src.cols / 2 - rect_size / 2 + 13, rect_size, rect_size);	 // (x, y, width, height)
	cropImg = src(r);

	Mat output1, output2, output3;
	window.copyTo(output1);
	window.copyTo(output2);
	window.copyTo(output3);

	for (int i = 0; i < resizedImg.rows; i++) {
		for (int j = 0; j < resizedImg.cols; j++) {
			output1.at<uchar>(i, j) = resizedImg.at<uchar>(i, j);
			output2.at<uchar>(i + (src.rows / 2) - (resizedImg.rows/2), j + (src.cols / 2) - ((resizedImg.cols / 2))) = flipImg.at<uchar>(i, j);
		}
	}
	for (int i = 1; i < cropImg.rows; i++) {
		for (int j = 1; j < cropImg.cols; j++) {
			output3.at<uchar>(src.rows - i, src.cols - j) = cropImg.at<uchar>(cropImg.rows - i, cropImg.cols - j);
		}
	}

	/*  read image  */


	/*  display image  */
	imshow("Original", src);
	imshow("output #1", output1);
	imshow("output #2", output2);
	imshow("output #3", output3);
  
	waitKey(0);
}

