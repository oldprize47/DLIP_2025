/*------------------------------------------------------------------------------------------\
@ Deep Learning & Image Processing  by Young-Keun Kim - Handong Global University

Author          : Sangheon Park
Created         : 2025-04-08
Modified        : 2025-04-15
Language/ver	: C++
Course		: Deep Learning & Image Processing

Description     : DLIP_LAB2_21800275_SangheonPark.cpp
/------------------------------------------------------------------------------------------*/

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat backGround, loopImage;

int hmin = 95, hmax = 108, smin = 130, smax = 255, vmin = 100, vmax = 255;

int main()
{	
	VideoCapture cap(0);
	VideoWriter outputVideo, sourceVideo;

	bool bRec = 0;

	if (!cap.isOpened())	// if not success, exit the programm
	{
		cout << "Cannot open the video cam\n";
		return -1;
	}
	Mat image_disp, hsv, mask, dst, dst1, dst2, final_image;
	vector<vector<Point> > contours;
	
	while(1){
		cap.read(backGround);
		imshow("Source", backGround);
		char c = (char)waitKey(10);
		if (c == 27) {
			break;
		}
	}

	imwrite("backGround.jpg", backGround);

	mask = Mat::Mat(backGround.size(), CV_8UC3, Scalar(255,255,255));

	getchar();

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


		/******** Add Post-Processing such as morphology etc  ********/
		int element_shape = MORPH_ELLIPSE;		// elliptical filter, MORPH_RECT, MORPH_ELLIPSE, MORPH_CROSS
		int n = 5;								// 3X3 Kernel
		Mat element = getStructuringElement(element_shape, Size(n, n));
		morphologyEx(dst, dst, MORPH_CLOSE, element, Point(-1, -1), 4); // Closing 4 times
		morphologyEx(dst, dst, MORPH_DILATE, element, Point(-1, -1), 6); // Dilation 6 times

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
			//namedWindow("Contour", 0);
			//imshow("Contour", dst_out);

			/// Draw the Contour Box on Original Image ///
			
			Rect boxPoint = boundingRect(contours[maxArea_idx]);

			/// Continue Drawing the Contour Box  ///
			mask = Scalar(255, 255, 255);
			rectangle(mask, boxPoint, Scalar(0,0,0), FILLED);
		}

		dst1 = image_disp & mask;
		mask = ~mask;
		dst2 = backGround & mask;
		
		if (contours.size() > 0)
			final_image = dst1 + dst2;
		else
			final_image = image_disp;

		imshow("Final", final_image);

		if (!bRec) {
			outputVideo.open("Sample2.mp4",
				VideoWriter::fourcc('M', 'P', '4', 'V'),
				20.0, backGround.size(), true);
			sourceVideo.open("Source.mp4",
				VideoWriter::fourcc('M', 'P', '4', 'V'),
				20.0, backGround.size(), true);
			bRec = 1;
		}

		if (bRec) {
			sourceVideo.write(loopImage);
			outputVideo.write(final_image);
		}

		char c = (char)waitKey(10);
		if (c == 27) {
			outputVideo.release();
			sourceVideo.release();
			break;
		}
	} // end of for(;;)

	

	return 0;
}