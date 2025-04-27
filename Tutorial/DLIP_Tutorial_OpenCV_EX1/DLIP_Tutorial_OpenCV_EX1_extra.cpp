#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
    // Open video camera with index 0
    VideoCapture cap(0);

    // Check if the video camera is opened successfully
    if (!cap.isOpened())
    {
        cout << "Cannot open the video camera\n";
        return -1;
    }

    // Create a window to display the video feed
    namedWindow("MyVideo", WINDOW_AUTOSIZE);

    bool flipHorizontal = false;
    bool flag = false;
    while (true)
    {
        Mat frame;

        // Read a new frame from the video feed
        bool readSuccess = cap.read(frame);

        // Check if reading the frame was successful
        if (!readSuccess)
        {
            cout << "Cannot find a frame from the video stream\n";
            break;
        }

        /* Flip src image*/
        if (waitKey(30) == 'h' || waitKey(30) == 'H') {
            flag = !flag;
        }

        if (flag) {
            rotate(frame, frame, 1);
        }

        // Display the frame in the "MyVideo" window
        imshow("MyVideo", frame);

        // Wait for 30ms and check if the 'ESC' key is pressed
        if (waitKey(30) == 27)
        {
            cout << "ESC key is pressed by the user\n";
            break;
        }
    }

    return 0;
}
