#include "imgproc.hpp"
#include <opencv2/opencv.hpp>
#include "cap.h"

// 创建一个高分辨率图像进行绘制
cv::Mat create_high_res_cursor_image(int scale) {
    // 创建一个更高分辨率 (40 * scale x 40 * scale) 的图像，4 通道（RGBA），初始化为全透明
    cv::Mat highResImage(40 * scale, 40 * scale, CV_8UC4, cv::Scalar(0, 0, 0, 0));

    int centerX = 20 * scale;
    int centerY = 20 * scale;
    int radius = 10 * scale;

    // 绘制实心圆
    cv::circle(highResImage, cv::Point(centerX, centerY), radius, cv::Scalar(255, 255, 255, 128), -1, cv::LINE_AA);

    // 绘制圆的边缘
    cv::circle(highResImage, cv::Point(centerX, centerY), radius, cv::Scalar(30, 30, 30, 255), scale, cv::LINE_AA);

    return highResImage;
}

cv::Mat create_cursor_image() {
    int scale = 8;
    // 获取高分辨率的图像
    cv::Mat highResImage = create_high_res_cursor_image(scale);

    // 缩小到 40x40，使用面积插值
    cv::Mat cursorImage;
    cv::resize(highResImage, cursorImage, cv::Size(40, 40), 0, 0, cv::INTER_AREA);

    // 绘制十字架（在缩小后的图像上）
    int centerX = 20;
    int centerY = 20;

    // 绘制垂直十字架
    cv::line(cursorImage, cv::Point(centerX, 0), cv::Point(centerX, 39), cv::Scalar(30, 30, 30, 255), 1, cv::LINE_AA);

    // 绘制水平十字架
    cv::line(cursorImage, cv::Point(0, centerY), cv::Point(39, centerY), cv::Scalar(30, 30, 30, 255), 1, cv::LINE_AA);

    return cursorImage;
}

cv::Mat get_fullscreen()
{
    cv::Mat res;

    int width, height, channels;
    unsigned char* buffer = captureFullScreen(&width, &height, &channels);
    if (buffer) {
        std::cout << "Captured screen: " << width << "x" << height << " with " << channels << " channels." << std::endl;
        cv::Size size;
        size.height = height;
        size.width = width;
        cv::Mat image(size, CV_8UC(channels), buffer);
        res = image.clone();
        free(buffer);
    } else {
        std::cerr << "Failed to capture screen" << std::endl;
    }

    return res;
}
