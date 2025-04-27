#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	/*  read src  */
	Mat img = imread("../../Image/HGU_logo.jpg");
	imshow("img", img);

	/* Flip src image*/
	Mat flipImg;
	rotate(img, flipImg, 1);
	imshow("flipImg", flipImg);

	/*  Crop(Region of Interest)  original image */
	// Add code here and show image


	/*  Show source(src) and destination(dst)  */
	// Add code here
	waitKey(0);
}