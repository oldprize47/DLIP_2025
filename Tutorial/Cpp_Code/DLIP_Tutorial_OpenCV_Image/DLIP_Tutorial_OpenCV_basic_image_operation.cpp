#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	/*  read image  */
	Mat img = imread("../../Image/HGU_logo.jpg");
	imshow("img", img);

	/*  Crop(Region of Interest)  */
	Rect r(img.cols/4, img.cols/4, img.cols / 2, img.cols / 2);	 // (x, y, width, height)
	Mat roiImg = img(r);
	imshow("roiImg", roiImg);

	/*  Rotate  */
	Mat rotImg;
	rotate(img, rotImg, ROTATE_90_CLOCKWISE);
	imshow("rotImg", rotImg);

	/*  Resize  */
	Mat resizedImg;
	resize(img, resizedImg, Size(1000, 100));
	imshow("resizedImg", resizedImg);

	waitKey(0);
}