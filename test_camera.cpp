#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Cannot open camera\n";
        return -1;
    }
    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Failed to capture frame\n";
            break;
        }
        cv::imshow("Camera Test", frame);
        if (cv::waitKey(30) == 27) break;  // Exit on ESC
    }
    return 0;
}
