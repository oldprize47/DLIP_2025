//#include <iostream>
//#include <opencv2/opencv.hpp>
//
//using namespace std;
//using namespace cv;
//
//int main() {
//    /*  read image  */
//    Mat img = imread("../../Image/HGU_logo.jpg");
//
//    if (img.empty())  /// Load image check
//    {
//        cout << "File Read Failed : src is empty" << endl;
//        waitKey(0);
//    }
//
//    imshow("img", img);
//
//    /*  Crop(Region of Interest)  */
//    Rect r(0, 100, 200, 200);  // (x, y, width, height)
//    Mat roiImg = img(r);
//    imshow("roiImg", roiImg);
//
//    /*  Rotate  */
//    Mat rotImg;
//    rotate(img, rotImg, ROTATE_90_CLOCKWISE);
//    imshow("rotImg", rotImg);
//
//    /*  Resize  */
//    Mat resizedImg;
//    resize(img, resizedImg, Size(1000, 100));
//    imshow("resizedImg", resizedImg);
//
//    waitKey(0);
//}

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//int main() {
//
//    Mat img, img_gray;
//    img = imread("../../Image/HGU_logo.jpg");
//    cvtColor(img, img_gray, COLOR_BGR2GRAY);
//    //image.at<uchar>(v, u) = 255;
//    //image.at<float>(v, u) = 0.9;
//
//    // For an RGB Image
//    // (option1) Vec3b: 8-bit 3-D image (RGB)
//    //image.at<cv::Vec3b>(v, u)[0] = 255;
//    //image.at<cv::Vec3b>(v, u)[1] = 255;
//    //image.at<cv::Vec3b>(v, u)[2] = 255;
//
//    /* Method 1. Accessing using "at<type>(v, u)" */
//    // For single channel image(Gray-scale)
//    printf("Gray color = %d\n", img_gray.at<uchar>(200, 200));
//
//    // For RGB image
//    printf("Blue color = %d\n", img.at<Vec3b>(200, 200)[0]);
//    printf("Green color = %d\n", img.at<Vec3b>(200, 200)[1]);
//    printf("Red color = %d\n", img.at<Vec3b>(200, 200)[2]);
//
//    /* Method 2. Accessing Using Pointer */
//    // Gray Image
//    int pixel_temp;
//    //for (int v = 0; v < img_gray.rows; v++) {
//        uchar* img_data = img_gray.ptr<uchar>(200);
//        //for (int u = 0; u < img_gray.cols; u++) {
//        pixel_temp = img_data[200];
//        cout << "(" << "200" << ", " << "200" << ") = " << pixel_temp << endl;
//            //cout << "(" << v << ", " << u << ") = " << pixel_temp  << endl;
//        //}
//            
//    //}
//
//    // RGB Image
//    int pixel_temp_r, pixel_temp_g, pixel_temp_b;
//    int cnt = 0;
//
//    for (int v = 0; v < img.rows; v++) {
//        uchar* img_data = img.ptr<uchar>(v);
//        for (int u = 0; u < img.cols * img.channels(); u = u + 3) {
//            pixel_temp_r = img_data[u];
//            pixel_temp_g = img_data[u + 1];
//            pixel_temp_b = img_data[u + 2];
//        }
//    }
//
//    return 0;
//}
    //
int main() {
    // Open video camera with index 0
    VideoCapture cap(0);

    // Check if the video camera is opened successfully
    if (!cap.isOpened()) {
        cout << "Cannot open the video camera\n";
        return -1;
    }

    // Create a window to display the video feed
    namedWindow("MyVideo", WINDOW_AUTOSIZE);

    bool flipHorizontal = false;
    bool rotate_flag = 0;

    while (true) {
        Mat frame, rot_frame;

        // Read a new frame from the video feed
        bool readSuccess = cap.read(frame);
        
        // Check if reading the frame was successful
        if (!readSuccess) {
            cout << "Cannot find a frame from the video stream\n";
            break;
        }

        rotate(frame, rot_frame, ROTATE_180);

        // Add code here
        if (waitKey(5) == 104) {
            cout << "h key is pressed by the user\n";
            rotate_flag = !rotate_flag;
        }

        if (rotate_flag) {
            imshow("MyVideo", rot_frame);
        } else {
            imshow("MyVideo", frame);
        }

        // Wait for 30ms and check if the 'ESC' key is pressed
        if (waitKey(5) == 27) {
            cout << "ESC key is pressed by the user\n";
            break;
        }
    }

    return 0;
}
