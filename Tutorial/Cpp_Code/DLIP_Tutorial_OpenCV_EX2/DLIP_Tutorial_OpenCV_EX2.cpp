#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
    // Load the image in gray-scale
    Mat src = imread("../../Image/HGU_logo.jpg", 0);

    if (src.empty())
    {
        cout << "Error: Couldn't open the image." << endl;
        return -1;
    }

    // Calculate the sum of pixel intensities using 'at' function
    double sumIntensity = 0.0;
    for (int i = 0; i < src.rows; i++){
        for (int j = 0; j < src.cols; ++j) {
            sumIntensity += src.at<uchar>(i, j);
        }
    }

    // Calculate the total number of pixels in the image
    int pixelCount = src.rows * src.cols; // Add code here

    // Calculate the average intensity of the image
    double avgIntensity = sumIntensity / pixelCount;// Add code here

    // Print the results
    cout << "Sum of intensity: " << sumIntensity << endl;
    cout << "Number of pixels: " << pixelCount << endl;
    cout << "Average intensity: " << avgIntensity << endl;

    // Display the gray-scale image
    imshow("src", src);
    waitKey(0);

    return 0;
}
