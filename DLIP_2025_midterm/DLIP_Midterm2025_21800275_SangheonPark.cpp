/*------------------------------------------------------/
* Image Proccessing with Deep Learning - DLIP
*
* MIDTERM 2025
*
* Date: 2025-04-18
*
* Name: Sangheon Park
*
* ID: 21800275
*
------------------------------------------------------*/

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	/******** Declaration of variables ********/
	Mat src, mask1, mask2, morp, inrange, hsv, blurred, dst1, dst2, final, mask_bound;
	int hmin = 0, hmax = 16, smin = 48, smax = 243, vmin = 48, vmax = 255;	// Inrange Value
	int face_num = 0;
	vector<vector<Point> > contours;

	/******** Read src image ********/
	src = imread("Test1_image_2025.jpg");
	// Load image check
	if (src.empty())
	{
		cout << "File Read Failed : src is empty" << endl;
		waitKey(0);
	}

	// Display source image
	imshow("Source image", src);

	/******** Color segmentation about cloak			 ********/
	// convert BGR to HSV
	cvtColor(src, hsv, COLOR_BGR2HSV);
	inRange(hsv, Scalar(MIN(hmin, hmax), MIN(smin, smax), MIN(vmin, vmax)),
		Scalar(MAX(hmin, hmax), MAX(smin, smax), MAX(vmin, vmax)), inrange);

	/******** Post-processing(e.g. Morphology...) ********/
	// Mask outside ROI
	rectangle(inrange, Rect(inrange.cols * 99 / 100, 0, inrange.cols / 100, inrange.rows * 1 / 3), Scalar(0, 0, 0), FILLED);
	rectangle(inrange, Rect(0, 0, inrange.cols / 100, inrange.rows * 1 / 3), Scalar(0, 0, 0), FILLED);

	// Morphology
	int element_shape = MORPH_ELLIPSE;		// elliptical filter, MORPH_RECT, MORPH_ELLIPSE, MORPH_CROSS
	int n = 3;								// 3X3 Kernel
	Mat element = getStructuringElement(element_shape, Size(n, n));
	morphologyEx(inrange, morp, MORPH_OPEN, element, Point(-1, -1), 3);	// 3times Opening
	morphologyEx(morp, morp, MORPH_DILATE, element, Point(-1, -1), 2); // 2times Dilation

	// Display skin-color-segmented output
	imshow("Task a) Segment skin-colored area", morp);

	/******** Create and Display Mask for Faces ********/
	findContours(morp, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	mask1 = Mat::Mat(src.size(), CV_8UC3, Scalar(255, 255, 255));	// Ready to make mask
	double minArea = 1900.0;	// Minimum contour size for face
	src.copyTo(mask_bound);

	// Check all contours
	for (int i = 0; i < contours.size(); i++) {
		if (contourArea(contours[i]) > minArea) {	// if Contour Size >  Minimum contour size for face
			Rect boxPoint = boundingRect(contours[i]);
			rectangle(mask1, boxPoint, Scalar(0, 0, 0), FILLED);	// Make rect shape mask
			rectangle(mask_bound, boxPoint, Scalar(255, 255, 255), 2);	// Make rect shape box
			face_num++;		// Caculate face num
		}
	}

	// Print how many faces found
	cout << "----------------------------------------------------------" << endl;
	cout << endl << endl << endl;
	cout << "Face Number: " << face_num << endl;
	cout << endl << endl << endl;
	cout << "----------------------------------------------------------" << endl;

	// Display mask images (binary)
	mask2 = ~mask1;	// Make mask2 for blur Image
	imshow("Task b) Create mask for faces", mask2);

	// Display mask boundary on source image
	imshow("Task c) Draw mask boundary on source image", mask_bound);

	/******** Blurring Masked Area   ********/
	cv::blur(src, blurred, cv::Size(7, 7)); // Make blur image from src

	// Masking
	dst1 = src & mask1;
	dst2 = blurred & mask2;

	// Merge
	final = dst1 + dst2;

	/******** Display the final output   ********/
	imshow("Final d) Blur only the mask area", final);

	int key = waitKey(0);
	if (key == 27) {
		cout << "ESC key is pressed by user\n";
		return 0;
	}

	return 0;
}