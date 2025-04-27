/* ------------------------------------------------------ /
*Image Proccessing with Deep Learning
* DLIP_LAB1_21800275_SangheonPark.cpp
* Created : 2025 - Spring
------------------------------------------------------ */

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;
using namespace cv;

vector<vector<Point>> contours;
vector<Vec4i> hierarchy;

Mat src[4], drawing_with_text[4], drawing[4], dst[4];
string imgName[4] = { "Gear1", "Gear2", "Gear3", "Gear4" };

Point2f center;
float diameter = 0.0, radius = 0.0;
double mean_Area = 0, avg_Area = 0, contour_Area[20] = { 0 };
int teeth_Num = 0, defective_Teeth = 0;

void thresholding_and_Morphology(Mat& src, Mat& dst);
void gear_Teeth_Segmentation(Mat& dst);
void Find_mean_avg();
void defective_Gear_Teeth_Detection(Mat& dst, Mat& drawing, Mat& drawing_with_text);
void show_Img_Info(int i);

int main() {

	for (int i = 0; i < 4; i++) {
		src[i] = cv::imread(imgName[i] + ".jpg", 0);
		if (src[i].empty())
		{
			return EXIT_FAILURE;
		}
		thresholding_and_Morphology(src[i], dst[i]);

		// Result Thresholding_and_Morphology
		namedWindow(imgName[i] + " - temp1", WINDOW_AUTOSIZE);
		imshow(imgName[i] + " - temp1", dst[i]);
	}

	Mat src1 = src[0];
	for (int i = 0; i < 4; i++) {

		gear_Teeth_Segmentation(dst[i]);
		findContours(dst[i], contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0)); // RETR_EXTERNAL, RETR_LIST, RETR_CCOMP, RETR_TREE , RETR_FLOODFILL

		// Result Gear_Teeth_Segmentation
		namedWindow(imgName[i] + " - temp2", WINDOW_AUTOSIZE);
		imshow(imgName[i] + " - temp2", dst[i]);

		Find_mean_avg();

		cvtColor(src[i], dst[i], COLOR_GRAY2RGB); // Reset dst Image
		defective_Gear_Teeth_Detection(dst[i], drawing[i], drawing_with_text[i]);

		show_Img_Info(i);
	}
	waitKey(0);

	return 0;
}

// Functions
void thresholding_and_Morphology(Mat& src, Mat& dst) {
	int element_shape = MORPH_ELLIPSE;		// elliptical filter
	int n = 3;								// 3X3 Kernel
	Mat element = getStructuringElement(element_shape, Size(n, n));

	threshold(src, dst, 128, 255, THRESH_BINARY);	// Thresholded using binary threshold with a value of 128.
	morphologyEx(dst, dst, MORPH_CLOSE, element, Point(-1, -1), 5); // Closing 5 times
}

void gear_Teeth_Segmentation(Mat& dst) {
	// Finding a full contour of gears
	findContours(dst, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0)); // RETR_EXTERNAL, RETR_LIST, RETR_CCOMP, RETR_TREE , RETR_FLOODFILL
	minEnclosingCircle(contours[0], center, radius); // Finding a circle that covers the contour
	circle(dst, center, radius * 4 / 5 + 1, Scalar(0), FILLED);  // Resize the circle and cover the image
	diameter = (radius * 4 / 5 + 1) * 2;
}

void Find_mean_avg() {
	avg_Area = 0; // Average initialization 

	// Save each tooth size
	for (int j = 0; j < hierarchy.size(); j++) {
		contour_Area[j] = contourArea(contours[j]);
		avg_Area += contour_Area[j];
	}
	sort(&contour_Area[0], &contour_Area[hierarchy.size() - 1]);
	mean_Area = contour_Area[hierarchy.size() / 2];	// Find the median
	avg_Area = avg_Area / hierarchy.size();			// Find the average
}

void defective_Gear_Teeth_Detection(Mat& dst, Mat& drawing, Mat& drawing_with_text) {
	int width = dst.rows, height = dst.cols;
	drawing = Mat::zeros(width, height, CV_8UC3);
	drawing.copyTo(drawing_with_text);

	defective_Teeth = 0;
	for (int j = 0; j < hierarchy.size(); j++) {
		minEnclosingCircle(contours[j], center, radius);	// Find the center of each tooth = Red mark, Yellow circle mark
		if (contourArea(contours[j]) < (mean_Area - mean_Area / 15) || contourArea(contours[j]) > (mean_Area + mean_Area / 15)) { // If it's a bad tooth
			drawContours(drawing, contours, j, Scalar(0, 0, 255), 2);
			drawContours(drawing_with_text, contours, j, Scalar(0, 0, 255), 2);
			putText(drawing_with_text, to_string((int)contourArea(contours[j])), center, 1, 1, Scalar(0, 0, 255), 2, 8);
			circle(dst, center, radius + 5, Scalar(0, 255, 255), 4); // Yellow circle mark
			defective_Teeth++;
		}
		else {	// For normal teeth = Green mark
			drawContours(drawing, contours, j, Scalar(0, 255, 0), 2);
			drawContours(drawing_with_text, contours, j, Scalar(0, 255, 0), 2);
			putText(drawing_with_text, to_string((int)contourArea(contours[j])), center, 1, 1, Scalar(255, 255, 255), 2, 8);
		}
	}
}

void show_Img_Info(int i) {

	teeth_Num = hierarchy.size();

	cout << imgName[i] << endl;
	cout << "Teeth numbers = " << teeth_Num << endl;
	cout << "Avg. Teeth Area = " << (int)avg_Area << endl;
	cout << "Defective Teeth = " << defective_Teeth << endl;
	cout << "Diameter of the Gear = " << diameter << endl;
	cout << "Quality = ";
	if (defective_Teeth == 0) {
		cout << "PASS";
	}
	else {
		cout << "FAIL";
	}
	cout << endl << endl;

	namedWindow(imgName[i] + " - src", WINDOW_AUTOSIZE);
	imshow(imgName[i] + " - src", src[i]);

	namedWindow(imgName[i] + " - text", WINDOW_AUTOSIZE);
	imshow(imgName[i] + " - text", drawing_with_text[i]);

	namedWindow(imgName[i] + " - contour", WINDOW_AUTOSIZE);
	imshow(imgName[i] + " - contour", drawing[i]);

	namedWindow(imgName[i] + " - Output", WINDOW_AUTOSIZE);
	imshow(imgName[i] + " - Output", dst[i]);
}