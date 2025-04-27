/* ------------------------------------------------------ /
*Image Proccessing with Deep Learning
* DLIP_Assignment_21800275_SangheonPark.cpp
* Created : 2025 - Spring
------------------------------------------------------ */

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

void mask_Outside_ROI(Mat& dst);
void line_Detect(Mat& dst, vector<Vec2f>* lines);
void convert_Line2Point(vector<Vec2f>* lines, Point* pt1, Point* pt2);
Point find_Vanishing_Point(Point* pt1, Point* pt2);
void draw_Lines(Mat& dst, Point* pt1, Point& vanishing_Point);

int main(int argc, char** argv)
{
	// Declare the output variables
	Mat dst;

	// Loads an image
	string filename[2] = { "Lane_center.jpg", "Lane_changing.jpg" };

	for (int k = 0; k < 2; k++) {
		vector<Vec2f> lines[2];
		Point pt1[2], pt2[2];
		Mat src = imread(filename[k], IMREAD_GRAYSCALE);

		// Check if image is loaded fine
		if (src.empty()) {
			printf(" Error opening image\n");
			return -1;
		}
		// Edge detection
		Canny(src, dst, 200, 50, 3);
		mask_Outside_ROI(dst);

		line_Detect(dst, lines);

		convert_Line2Point(lines, pt1, pt2);
		Point vanishing_Point = find_Vanishing_Point(pt1, pt2);

		cvtColor(src, dst, COLOR_GRAY2BGR);
		draw_Lines(dst, pt1, vanishing_Point);

		// Show results
		imshow(filename[k] + " - src", src);
		imshow(filename[k] + " - dst", dst);

	}

	// Wait and Exit
	waitKey();
	return 0;
}

/*************************** Functions ***************************/
void mask_Outside_ROI(Mat& dst) {
	// Mask outside ROI
	rectangle(dst, Rect(0, 0, dst.cols, dst.rows / 1.7), Scalar(0), FILLED);
	rectangle(dst, Rect(0, 0, dst.cols / 5, dst.rows), Scalar(0), FILLED);
	rectangle(dst, Rect(dst.cols * 4 / 5, 0, dst.cols / 5, dst.rows), Scalar(0), FILLED);
}

void line_Detect(Mat& dst, vector<Vec2f>* lines) {
	// Standard Hough Line Transform
	HoughLines(dst, lines[0], 1, CV_PI / 180, 44, 0, 0, 20 * CV_PI / 180, 60 * CV_PI / 180);
	HoughLines(dst, lines[1], 1, CV_PI / 180, 70, 0, 0, 290 * CV_PI / 180, 330 * CV_PI / 180);
}

void convert_Line2Point(vector<Vec2f>* lines, Point* pt1, Point* pt2) {
	// j = 0 -> Left Line, j = 1 -> :Right Line
	for (int j = 0; j < 2; j++) {
		// Draw the detected lines
		float rho = 0, theta = 0;
		for (size_t i = 0; i < lines[j].size(); i++)
		{
			rho += lines[j][i][0];
			theta += lines[j][i][1];
		}
		rho /= lines[j].size();
		theta /= lines[j].size();

		cout << "[" << j << "]" << "rho = " << rho << endl;
		cout << "[" << j << "]" << "theta = " << theta * 180 / CV_PI << endl;

		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;

		// ptr1 -> low point
		pt1[j].x = cvRound(x0 + 1500 * (-b));
		pt1[j].y = cvRound(y0 + 1500 * (a));

		// ptr1 -> hight point
		pt2[j].x = cvRound(x0 - 1500 * (-b));
		pt2[j].y = cvRound(y0 - 1500 * (a));
	}
}

Point find_Vanishing_Point(Point* pt1, Point* pt2) {

	Point2f p1(pt1[0].x, pt1[0].y), p2(pt2[0].x, pt2[0].y), p3(pt1[1].x, pt1[1].y), p4(pt2[1].x, pt2[1].y);
	double A1 = p2.y - p1.y, A2 = p4.y - p3.y;
	double B1 = p1.x - p2.x, B2 = p3.x - p4.x;
	double C1 = A1 * p1.x + B1 * p1.y, C2 = A2 * p3.x + B2 * p3.y;

	Matx22f A(A1, B1,
		A2, B2);
	Vec2f B(C1, C2);
	Vec2f solution;

	bool success = solve(A, B, solution, DECOMP_LU);

	if (success) {
		std::cout << "Intersection point: (" << solution[0] << ", " << solution[1] << ")" << std::endl;
	}
	else {
		std::cout << "No intersection (lines are parallel or coincident)" << std::endl;
	}

	return Point(solution[0], solution[1]);
}

void draw_Lines(Mat& dst, Point* pt1, Point& vanishing_Point) {
	for (int j = 0; j < 2; j++) {
		if (j % 2 == 0) {
			line(dst, pt1[j], vanishing_Point, Scalar(0, 0, 255), 3, LINE_AA);
		}
		else {
			line(dst, pt1[j], vanishing_Point, Scalar(0, 255, 0), 3, LINE_AA);
		}
	}

	circle(dst, vanishing_Point, 5, Scalar(203, 192, 255), 2);
	Point tri_Point[1][3] = { pt1[0], pt1[1], vanishing_Point };
	const Point* ppt[1] = { tri_Point[0] };
	int npt[] = { 3 };
	Mat tri = Mat::zeros(dst.rows, dst.cols, CV_8UC3);
	fillPoly(tri, ppt, npt, 1, Scalar(0, 0, 255));
	addWeighted(tri, 0.2, dst, 1, 0, dst);
	line(dst, vanishing_Point, Point(vanishing_Point.x, vanishing_Point.y + dst.rows), Scalar(255, 0, 0), 1, LINE_AA);
}