#include "cap.h"
#include <iostream>
#include <opencv2/opencv.hpp>

#include <GLFW/glfw3.h>

void save_image(cv::Mat& image)
{
    cv::Mat res;
    cv::cvtColor(image, res, cv::COLOR_RGBA2BGRA);
    //cv::imwrite("screenshot.png", image);
}

struct Config
{
    Config(bool a_showTitleBar, bool a_showBorders):
        showTitleBar(a_showTitleBar),
        showBorders(a_showBorders)
    {}

    bool showTitleBar = false;
    bool showBorders = false;
};

// get RGBA image
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

// Callback function for GLFW errors
void error_callback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

int main() {
    Config config(false, false);

    cv::Mat src = get_fullscreen();
    cv::Mat image;
    cv::flip(src, image, 0); // vertical flip

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    int width = image.cols;
    int height = image.rows;

    // 获取主显示器
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    // 配置窗口装饰
    glfwWindowHint(GLFW_DECORATED, config.showTitleBar && config.showBorders ? GLFW_TRUE : GLFW_FALSE);

    // Set the error callback
    glfwSetErrorCallback(error_callback);
    printf("primary monitor: width=%d, height=%d\n", mode->width, mode->height);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Screen Capture", primaryMonitor, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set up the projection matrix to flip the Y-axis
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, image.cols, 0, image.rows, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glRasterPos2i(0, 0);
        glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, image.data);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
