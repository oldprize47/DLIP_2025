#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	Mat src, gray;

	String filename[2] = { "TrafficSign1.png", "eyepupil.png" };
	//String filename = "eyepupil.png";
	/* Read the image */

	for (int i = 0; i < 2; i++) {

		src = imread("./Image/" + filename[i], 1);

		if (!src.data)
		{
			printf(" Error opening image\n");
			return -1;
		}

		cvtColor(src, gray, COLOR_BGR2GRAY);

		/* smooth it, otherwise a lot of false circles may be detected */
		//GaussianBlur(gray, gray, Size(3, 3), 1, 1);
		//blur(gray, gray, Size(3, 3));

		vector<Vec3f> circles;
		HoughCircles(gray, circles, HOUGH_GRADIENT, 2, gray.cols, 200, 100, gray.cols / 20, gray.rows / 4);
		for (size_t i = 0; i < circles.size(); i++)
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);

			/* draw the circle center */
			circle(src, center, 3, Scalar(0, 255, 0), -1, 8, 0);

			/* draw the circle outline */
			circle(src, center, radius, Scalar(0, 0, 255), 3, 8, 0);
			cout << "detect" << endl;
		}

		namedWindow(filename[i], 1);
		imshow(filename[i], src);
	}
	/* Wait and Exit */
	waitKey();
	return 0;
}
