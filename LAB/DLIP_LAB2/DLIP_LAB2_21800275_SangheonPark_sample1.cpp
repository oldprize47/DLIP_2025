/*------------------------------------------------------------------------------------------\
@ Deep Learning & Image Processing  by Young-Keun Kim - Handong Global University

Author          : Sangheon Park
Created         : 2025-04-08
Modified        : 2025-04-13
Language/ver	: C++
Course		: Deep Learning & Image Processing

Description     : DLIP_LAB2_21800275_SangheonPark.cpp
/------------------------------------------------------------------------------------------*/
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void mophology(Mat& dst);
Rect find_MaxArea_and_Get_BoxPoint(vector<vector<Point>>& contours);
Mat make_Mask(Rect& boxPoint, Mat& image);
void bitwise_and_Merge(Mat& image_disp, Mat& backGround, Mat& mask, Mat& final_image);

Mat backGround, loopImage;
int hmin = 16, hmax = 17, smin = 100, smax = 255, vmin = 100, vmax = 255;

int main()
{
	VideoCapture cap("LAB_MagicCloak_Sample1.mp4");
	VideoWriter outputVideo;	// for Recording
	Mat image_disp, hsv, mask, dst, dst1, dst2, final_image;
	vector<vector<Point> > contours;
	bool bRec = 0;

	if (!cap.isOpened())	// if not success, exit the programm
	{
		cout << "Cannot open the video cam\n";
		return -1;
	}

	cap.read(backGround);

	while (true)
	{

		bool bSuccess = cap.read(loopImage);
		if (!bSuccess)	// if not success, break loop
		{
			cout << "Cannot find a frame from  video stream\n";
			break;
		}

		loopImage.copyTo(image_disp);
		imshow("Source", image_disp);

		/******** Convert BGR to HSV ********/
		// input mat: image
		// output mat: hsv
		cvtColor(image_disp, hsv, COLOR_BGR2HSV);

		/// set dst as the output of InRange
		inRange(hsv, Scalar(MIN(hmin, hmax), MIN(smin, smax), MIN(vmin, vmax)),
			Scalar(MAX(hmin, hmax), MAX(smin, smax), MAX(vmin, vmax)), dst);

		mophology(dst);

		///  Find All Contour   ///
		findContours(dst, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		if (contours.size() > 0)
		{
			Rect boxPoint = find_MaxArea_and_Get_BoxPoint(contours);

			Mat mask = make_Mask(boxPoint, image_disp);

			bitwise_and_Merge(loopImage, backGround, mask, final_image);
		}
		else {
			final_image = loopImage;
		}

		imshow("Final", final_image);

		/*****************	for Recording	*******************/
		if (!bRec) {
			outputVideo.open("Sample1_1.mp4",
				VideoWriter::fourcc('M', 'P', '4', 'V'),
				30.0, backGround.size(), true);
			bRec = 1;
		}

		if (bRec)
			outputVideo.write(final_image);
		/******************************************************/

		char c = (char)waitKey(10);
		if (c == 27)
			break;
	} // end of for(;;)

	outputVideo.release();

	return 0;
}

void mophology(Mat& dst) {
	int element_shape = MORPH_ELLIPSE;		// elliptical filter, MORPH_RECT, MORPH_ELLIPSE, MORPH_CROSS
	int n = 5;								// 5X5 Kernel
	Mat element = getStructuringElement(element_shape, Size(n, n));
	morphologyEx(dst, dst, MORPH_CLOSE, element, Point(-1, -1), 4); // Closing 4 times
	morphologyEx(dst, dst, MORPH_DILATE, element, Point(-1, -1), 5); // Dilation 5 times
}
Rect find_MaxArea_and_Get_BoxPoint(vector<vector<Point>>& contours) {
	/// Find the Contour with the largest area ///
	double maxArea = 0;
	int maxArea_idx = 0;

	for (int i = 0; i < contours.size(); i++)
		if (contourArea(contours[i]) > maxArea) {
			maxArea = contourArea(contours[i]);
			maxArea_idx = i;
		}

	/// Return Box Point ///
	return boundingRect(contours[maxArea_idx]);
}
Mat make_Mask(Rect& boxPoint, Mat& image) {
	Mat mask = Mat::Mat(image.size(), CV_8UC3, Scalar(255, 255, 255));
	/// Continue Drawing the Contour Box on Mask  ///
	mask = Scalar(255, 255, 255);
	rectangle(mask, boxPoint, Scalar(0, 0, 0), FILLED);

	return mask;
}

void bitwise_and_Merge(Mat& loopImage, Mat& backGround, Mat& mask, Mat& final_image) {
	Mat dst1 = loopImage & mask;
	mask = ~mask;
	Mat dst2 = backGround & mask;
	final_image = dst1 + dst2;
}