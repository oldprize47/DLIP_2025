/*------------------------------------------------------/
* Image Proccessing with Deep Learning
* OpenCV : Threshold using Trackbar Demo
* Created: 2021-Spring
------------------------------------------------------*/

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// Global variables for Threshold
int threshold_value = 0;
int threshold_type = 0;
int img_num = 0;
int morphology_type = 0;

int const max_value = 255;
int const max_type = 4;
int const img_type = 8;
int const max_BINARY_value = 255;

// Global variables for Morphology
int element_shape = MORPH_RECT;		// MORPH_RECT, MORPH_ELIPSE, MORPH_CROSS
int n = 3;
Mat element = getStructuringElement(element_shape, Size(n, n));

Mat src, src_gray, dst, dst_morph;

// Trackbar strings
String window_name = "Threshold & Morphology Demo";
String trackbar_type = "TH Type:";// 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Invert";
String trackbar_value = "TH Value";
String trackbar_morph = "Morph Type"; // 0: None \n 1: erode \n 2: dilate \n 3: close \n 4: open";
string imgName[9] = {  "barcode2.jpg", "coin.jpg", "Finger_print_gray.tif",
"localThresh1.jpg", "localThresh2.jpg", "localThresh3.jpg", "rice.png",
"roadshadow.JPG", "Septagon_noisy.tif"};

// Function headers
void Threshold_Demo(int, void*);
void Morphology_Demo(int, void*);
void Img_Demo(int, void*);

int main()
{
	// Load an image
	//src = imread("../../Image/Finger_print_gray.tif", IMREAD_COLOR);
	src = imread("../../Image/testImage/" + imgName[0], IMREAD_COLOR);
	if (src.empty())
	{
		return EXIT_FAILURE;
	}

	// Convert the image to Gray
	cvtColor(src, src_gray, COLOR_BGR2GRAY);

	// Create a window to display the results
	namedWindow(window_name, WINDOW_NORMAL);

	// Create trackbar to choose type of threshold
	createTrackbar("Image_Num:", window_name, &img_num, img_type, Img_Demo);
	createTrackbar(trackbar_type, window_name, &threshold_type, max_type, Threshold_Demo);
	createTrackbar(trackbar_value, window_name, &threshold_value, max_value, Threshold_Demo);
	createTrackbar(trackbar_morph, window_name, &morphology_type, max_type, Morphology_Demo);

	// Call the function to initialize
	Img_Demo(0, 0);
	Threshold_Demo(0, 0);
	Morphology_Demo(0, 0);

	// Wait until user finishes program
	while (true) {
		int c = waitKey(20);
		if (c == 27)
			break;
	}
}

void Img_Demo(int, void*)	// default form of callback function for trackbar
{
	src = imread("../../Image/testImage/" + imgName[img_num], IMREAD_COLOR);
	// Convert the image to Gray
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	imshow(window_name, src_gray);
}

void Threshold_Demo(int, void*)	// default form of callback function for trackbar
{
	/*
	* 0: Binary
	* 1: Threshold Truncated
	* 2: Threshold to Zero
	* 3: Threshold to Zero Inverted
	*/

	threshold(src_gray, dst, threshold_value, max_BINARY_value, threshold_type);
	imshow(window_name, dst);
}

void Morphology_Demo(int, void*)  // default form of callback function for trackbar
{
	/*
	* 0: None
	* 1: Erode
	* 2: Dilate
	* 3: Close
	* 4: Open
	*/

	switch (morphology_type) {
	case 0: dst.copyTo(dst_morph);	break;
	case 1: erode(dst, dst_morph, element); break;
	case 2: dilate(dst, dst_morph, element); break;
	case 3: morphologyEx(dst, dst_morph, MORPH_OPEN, element); break;
	case 4: morphologyEx(dst, dst_morph, MORPH_CLOSE, element); break;
	}
	imshow(window_name, dst_morph);
}