#include "cap.h"
#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    int width, height, channels;
    unsigned char* buffer = captureFullScreen(&width, &height, &channels);
    if (buffer) {
        std::cout << "Captured screen: " << width << "x" << height << " with " << channels << " channels." << std::endl;
        cv::Size size;
        size.height = height;
        size.width = width;
        cv::Mat image(size, CV_8UC(channels), buffer);
        cv::cvtColor(image, image, cv::COLOR_RGBA2BGRA);
        cv::imwrite("screenshot.png", image);

        // Use the buffer as needed
        free(buffer);
    } else {
        std::cerr << "Failed to capture screen" << std::endl;
    }
    return 0;
}