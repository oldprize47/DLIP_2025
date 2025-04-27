#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
    // Load the image
    Mat src = imread("../../Image/HGU_logo.jpg", 0);

    if (src.empty())
    {
        cout << "Error: Couldn't open the image." << endl;
        return -1;
    }

    // Initialize dst with the same size as srcGray
    Mat dst = Mat::zeros(src.size(), src.type());

    // Invert the colors by accessing each pixel
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            dst.at<uchar>(i, j) = 255 - src.at<uchar>(i, j);
        }
    }

    // Display the original and inverted images
    imshow("src", src);
    imshow("dst", dst);
    waitKey(0);

    return 0;
}
