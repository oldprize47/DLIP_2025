#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	/*  read src  */
	//Mat img = imread("../../../Image/HGU_logo.jpg");
    Mat img = imread("Capture_20260401_153900_069.jpg");
	imshow("img", img);

	/* Flip src image*/
	Mat flipImg;
    rotate(img, flipImg, 2);
	imshow("flipImg", flipImg);


	//*********************************//
    Mat img_rot;

    Point2f center(img.cols / 2.0, img.rows / 2.0);  // Center of rotation

    double angle = -45.0;  // Rotation angle in degrees (negative for clockwise)
    double scale = 1.0;    // Scale factor (1.0 means no scaling)

    Mat rot_mat = getRotationMatrix2D(center, angle, scale);  // Get rotation matrix

    warpAffine(img, img_rot, rot_mat, img.size());  // Rotate the image

    imshow("rotated_img", img_rot);

	/*  Crop(Region of Interest)  original image */
	// Add code here and show image


	/*  Show source(src) and destination(dst)  */
	// Add code here
	waitKey(0);
}