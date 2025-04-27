#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat backGround, loopImage;
Point origin;
Rect selection;
bool selectObject = false;
bool trackObject = false;
int hmin = 16, hmax = 17, smin = 100, smax = 255, vmin = 100, vmax = 255;
/// On mouse event 
static void onMouse(int event, int x, int y, int, void*);

int main()
{	
	VideoCapture cap("LAB_MagicCloak_Sample1.mp4");
	VideoWriter outputVideo;

	bool bRec = 0;

	if (!cap.isOpened())	// if not success, exit the programm
	{
		cout << "Cannot open the video cam\n";
		return -1;
	}
	Mat image_disp, hsv, mask1, mask2, dst, dst1, dst2, final_image;
	vector<vector<Point> > contours;

	//image = imread("color_ball.jpg");
	//image.copyTo(image_disp);
	cap.read(backGround);

	mask1 = Mat::Mat(backGround.size(), CV_8UC3, Scalar(255,255,255));
	mask2 = Mat::Mat(backGround.size(), CV_8UC3, Scalar(0, 0, 0));
	// TrackBar 설정
	namedWindow("Source", 0);
	setMouseCallback("Source", onMouse, 0);
	createTrackbar("Hmin", "Source", &hmin, 179, 0);
	createTrackbar("Hmax", "Source", &hmax, 179, 0);
	createTrackbar("Smin", "Source", &smin, 255, 0);
	createTrackbar("Smax", "Source", &smax, 255, 0);
	createTrackbar("Vmin", "Source", &vmin, 255, 0);
	createTrackbar("Vmax", "Source", &vmax, 255, 0);


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
		cvtColor(image_disp, hsv, COLOR_BGR2HSV); //// YOUR CODE GOES HERE


		/******** Add Pre-Processing such as filtering etc  ********/
		// YOUR CODE GOES HERE
		// YOUR CODE GOES HERE



		/// set dst as the output of InRange
		inRange(hsv, Scalar(MIN(hmin, hmax), MIN(smin, smax), MIN(vmin, vmax)),
			Scalar(MAX(hmin, hmax), MAX(smin, smax), MAX(vmin, vmax)), dst);


		/******** Add Post-Processing such as morphology etc  ********/
		int element_shape = MORPH_ELLIPSE;		// elliptical filter, MORPH_RECT, MORPH_ELLIPSE, MORPH_CROSS
		int n = 5;								// 3X3 Kernel
		Mat element = getStructuringElement(element_shape, Size(n, n));
		morphologyEx(dst, dst, MORPH_CLOSE, element, Point(-1, -1), 4); // Closing 5 times
		morphologyEx(dst, dst, MORPH_DILATE, element, Point(-1, -1), 5);

		namedWindow("InRange", 0);
		imshow("InRange", dst);

		/// once mouse has selected an area bigger than 0
		if (trackObject)
		{
			trackObject = false;					// Terminate the next Analysis loop
			Mat roi_HSV(hsv, selection); 			// Set ROI by the selection box		
			Scalar means, stddev;
			meanStdDev(roi_HSV, means, stddev);
			cout << "\n Selected ROI Means= " << means << " \n stddev= " << stddev;

			// Change the value in the trackbar according to Mean and STD //
			//hmin = MAX((means[0] - stddev[0]), 0);
			//hmax = MIN((means[0] + stddev[0]), 179);
			setTrackbarPos("Hmin", "Source", hmin);
			setTrackbarPos("Hmax", "Source", hmax);

			/******** Repeat for S and V trackbar ********/
			setTrackbarPos("Smin", "Source", smin);
			setTrackbarPos("Smax", "Source", smax);
			setTrackbarPos("Vmin", "Source", vmin);
			setTrackbarPos("Vmax", "Source", vmax);
		}

		if (selectObject && selection.area() > 0)  // Left Mouse is being clicked and dragged
		{
			// Mouse Drag을 화면에 보여주기 위함
			Mat roi_RGB(image_disp, selection);
			bitwise_not(roi_RGB, roi_RGB);
			imshow("Source", image_disp);
		}
		loopImage.copyTo(image_disp);

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
			namedWindow("Contour", 0);
			imshow("Contour", dst_out);

			/// Draw the Contour Box on Original Image ///
			Rect boxPoint = boundingRect(contours[maxArea_idx]);

			/// Continue Drawing the Contour Box  ///
			mask1 = Scalar(255, 255, 255);
			mask2 = Scalar(0, 0, 0);
			rectangle(mask1, boxPoint, Scalar(0,0,0), FILLED);
			rectangle(mask2, boxPoint, Scalar(255,255,255), FILLED);

			dst1 = image_disp & mask1;
			dst2 = backGround & mask2;
			final_image = dst1 + dst2;

			imshow("Final", final_image);

			if (!bRec) {
				outputVideo.open("Sample1_1.mp4",
					VideoWriter::fourcc('M', 'P', '4', 'V'),
					30.0, backGround.size(), true);
				bRec = 1;
			}

			if (bRec)
				outputVideo.write(final_image);
		}

		char c = (char)waitKey(10);
		if (c == 27)
			break;
	} // end of for(;;)

	outputVideo.release();

	return 0;
}



/// On mouse event 
static void onMouse(int event, int x, int y, int, void*)
{
	if (selectObject)  // for any mouse motion
	{
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = abs(x - origin.x) + 1;
		selection.height = abs(y - origin.y) + 1;
		selection &= Rect(0, 0, loopImage.cols, loopImage.rows);  /// Bitwise AND  check selectin is within the image coordinate
	}

	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		selectObject = true;
		origin = Point(x, y);
		break;
	case EVENT_LBUTTONUP:
		selectObject = false;
		if (selection.area())
			trackObject = true;
		break;
	}
}
