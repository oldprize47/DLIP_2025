#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	Mat src;
	// read image  
	src = imread("../../Image/HGU_logo.jpg", 0);

	int v = src.rows; //행(가로)
	int u = src.cols; //열(세로)
	
	for (int i = 0; i < v; ++i) {
		cout << "\n";
		for (int j = 0; j < u; ++j)
			printf("%d ", src.at<uchar>(i, j));
	}

	imshow("src", src);

	waitKey(0);
}