#include "cap.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "imgproc.hpp"
#include <GLFW/glfw3.h>

bool isSelecting = false;
bool selected = false;
bool confirmed = false;
double startX=0, startY=0, currentX=0, currentY=0;
cv::Mat image;

// Callback function for GLFW errors
void error_callback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

cv::Rect get_rect(double x1, double y1, double x2, double y2)
{
    int x1i = static_cast<int>(x1);
    int y1i = static_cast<int>(y1);
    int x2i = static_cast<int>(x2);
    int y2i = static_cast<int>(y2);
    int xmin = std::min(x1i, x2i);
    int ymin = std::min(y1i, y2i);
    int xmax = std::max(x1i, x2i);
    int ymax = std::max(y1i, y2i);
    return cv::Rect(xmin, ymin, xmax-xmin+1, ymax-ymin+1);
}

// 鼠标按下回调函数
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isSelecting = true;
            selected = false;
            glfwGetCursorPos(window, &startX, &startY); // 使用最新的鼠标位置作为起点
            currentX = startX;
            currentY = startY;
        } else if (action == GLFW_RELEASE) {
            isSelecting = false;
            selected = true;
        }
    }
}

// 鼠标移动回调函数
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (isSelecting)
    {
        currentX = xpos;
        currentY = ypos;
    }
}

// 处理 framebuffer 大小变化（物理层面的分辨率）
void framebuffer_size_callback(GLFWwindow* window, int framebuffer_width, int framebuffer_height)
{
    // 调整视角与窗口尺寸一致
    glViewport(0, 0, framebuffer_width, framebuffer_height);
}

// 处理窗口大小变化（逻辑层面的分辨率）
// 注意，macOS 使用 RETINA 屏，物理分辨率的宽度、高度，分别是逻辑分辨率的宽度、高度的2倍
// 因此设置投影矩阵 和 viewport， 要分开设置
void window_size_callback(GLFWwindow* window, int width, int height)
{
    // std::cout << "Window size changed: " << width << "x" << height << std::endl;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

GLFWwindow* get_glfw_fullscreen_window()
{
    // 获取主显示器
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    printf("primary monitor: width=%d, height=%d\n", mode->width, mode->height);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Screen Capture", nullptr, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    // 将窗口移动到屏幕的左上角
    glfwSetWindowPos(window, 0, 0);

    return window;
}

GLFWwindow* get_simple_window(int image_width, int image_height)
{
//     int window_width = 800;
//     int window_height = 600;
    GLFWwindow* window = glfwCreateWindow(image_width, image_height, "Hello GLFW", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    return window;
}

void draw_textured_quad(GLuint texture, int target_image_width, int target_image_height)
{
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_TEXTURE_2D);

    // 绘制图像
    glColor3f(1.0f, 1.0f, 1.0f); // 设置白色以使用纹理的本色
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(static_cast<float>(target_image_width), 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(static_cast<float>(target_image_width), static_cast<float>(target_image_height));
    glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, static_cast<float>(target_image_height));
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// 为选中区域绘制蓝色边框，为非选中区域显示暗色遮罩
void draw_overlay(double x1, double y1, double x2, double y2, int window_width, int window_height)
{
    // 启用混合功能
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 设置灰色的半透明蒙版颜色
    glColor4f(0.1f, 0.1f, 0.1f, 0.5f);  // 50% 透明

    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);

    // 绘制左侧区域
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(x1, 0.0f);
    glVertex2f(x1, window_height);
    glVertex2f(0.0f, window_height);
    glEnd();

    // 绘制右侧区域
    glBegin(GL_QUADS);
    glVertex2f(x2, 0.0f);
    glVertex2f(window_width, 0.0f);
    glVertex2f(window_width, window_height);
    glVertex2f(x2, window_height);
    glEnd();

    // 绘制顶部区域
    glBegin(GL_QUADS);
    glVertex2f(x1, 0.0f);
    glVertex2f(x2, 0.0f);
    glVertex2f(x2, y1);
    glVertex2f(x1, y1);
    glEnd();

    // 绘制底部区域
    glBegin(GL_QUADS);
    glVertex2f(x1, y2);
    glVertex2f(x2, y2);
    glVertex2f(x2, window_height);
    glVertex2f(x1, window_height);
    glEnd();


    // 绘制蓝色边框
    glColor3f(0.2f, 0.5f, 1.0f); // 蓝色
    glBegin(GL_LINE_LOOP);
    glVertex2f(static_cast<float>(x1), static_cast<float>(y1));
    glVertex2f(static_cast<float>(x2), static_cast<float>(y1));
    glVertex2f(static_cast<float>(x2), static_cast<float>(y2));
    glVertex2f(static_cast<float>(x1), static_cast<float>(y2));
    glEnd();


    // 关闭混合以避免影响其他绘制操作
    glDisable(GL_BLEND);
}

// 按钮结构体
struct Button {
    float x, y, width, height;
    const char* label;
};

// 检查鼠标是否在按钮内
bool isMouseOverButton(Button button, double mouseX, double mouseY) {
    return mouseX >= button.x && mouseX <= button.x + button.width &&
           mouseY >= button.y && mouseY <= button.y + button.height;
}

// 鼠标按钮回调  
void mouse_button_callback_for_widget(GLFWwindow* window, int button, int action, int mods) {  
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {  
        double mouseX, mouseY;  
        glfwGetCursorPos(window, &mouseX, &mouseY);  

        // 获取窗口大小  
        int windowWidth, windowHeight;  
        glfwGetWindowSize(window, &windowWidth, &windowHeight);  

        // 翻转Y轴，因为OpenGL的坐标系原点在左下角  
        mouseY = windowHeight - mouseY;  

        // 获取按钮数组  
        Button* buttons = static_cast<Button*>(glfwGetWindowUserPointer(window));  

        // 检查按钮点击  
        if (isMouseOverButton(buttons[0], mouseX, mouseY)) {  
            std::cout << "Cancel button clicked!" << std::endl;  
            
        } else if (isMouseOverButton(buttons[1], mouseX, mouseY)) {  
            std::cout << "Confirm button clicked!" << std::endl;  

            int framebuffer_width, framebuffer_height;
            glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);

            int window_width, window_height;
            glfwGetWindowSize(window, &window_width, &window_height);

            float scale_width = framebuffer_width * 1.0f / window_width;
            float scale_height = framebuffer_height * 1.0f / window_height;

            cv::Rect rect = get_rect(startX * scale_width, startY * scale_height, currentX * scale_width, currentY * scale_height);

            cv::Mat roi = image(rect);
            cv::Mat res;
            cv::cvtColor(roi, res, cv::COLOR_RGBA2BGR);
            cv::imwrite("cap.png", res);
        }  
        confirmed = true;
    }  
}

// 绘制X形状  
void drawX(Button button) {  
    // 设置线条颜色  
    glColor3f(0.7f, 0.7f, 0.7f); 

    // 绘制两条对角线  
    glBegin(GL_LINES);  

    float x1 = button.x + 0.1 * button.width;
    float y1 = button.y + 0.1 * button.height;
    float x2 = button.x + 0.9 * button.width;
    float y2 = button.y + 0.9 * button.height;

    // 第一条对角线  
    glVertex2f(x1, y1);  
    glVertex2f(x2, y2);  

    // 第二条对角线  
    glVertex2f(x2, y1);
    glVertex2f(x1, y2);
    glEnd();  
}

// 绘制对勾形状  
void drawCheckMark(Button button) {  
    // 设置线条颜色
    glColor3f(0.8f, 0.8f, 0.8f);

    // 计算对勾的顶点  
    float startX = button.x + button.width * 0.1f;  
    float startY = button.y + button.height * 0.5f;  
    float midX = button.x + button.width * 0.4f;  
    float midY = button.y + button.height * 0.8f;  
    float endX = button.x + button.width * 0.8f;  
    float endY = button.y + button.height * 0.2f;  

    // 绘制对勾  
    glBegin(GL_LINES);  
    // 第一条线段  
    glVertex2f(startX, startY);  
    glVertex2f(midX, midY);  

    // 第二条线段  
    glVertex2f(midX, midY);  
    glVertex2f(endX, endY);  
    glEnd();  
}

void render_toolbox_window(GLFWwindow* widget_window, Button* buttons) {
    // // 设置背景颜色为白色
    //glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 绘制按钮
    drawX(buttons[0]);
    drawCheckMark(buttons[1]);
}

int main()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 配置窗口装饰
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    // Set the error callback
    glfwSetErrorCallback(error_callback);

    GLFWwindow* window = get_glfw_fullscreen_window();
//     cv::Mat image = cv::imread("/Users/zz/data/peppers.png");
//     cv::cvtColor(image, image, cv::COLOR_BGR2RGBA);

    GLFWwindow* widget_window = get_simple_window(200, 100);

    // 定义按钮
    Button buttons[2] = {
        {20, 20, 60, 60, "Cancel"},
        {120, 20, 60, 60, "Confirm"}
    };
    // 设置用户指针为按钮数组
    glfwSetWindowUserPointer(widget_window, buttons);
    glfwSetMouseButtonCallback(widget_window, mouse_button_callback_for_widget);

    image = get_fullscreen();
    printf("image size: width=%d, height=%d\n", image.cols, image.rows);
    //GLFWwindow* window = get_simple_window(image.cols, image.rows);

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 初始化窗口大小 (逻辑分辨率）
    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);
    window_size_callback(window, window_width, window_height);
    printf("window size: width=%d, height=%d\n", window_width, window_height);

    // 初始化 viewport (物理分辨率）
    int framebuffer_width, framebuffer_height;
    glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
    framebuffer_size_callback(window, framebuffer_width, framebuffer_height);
    printf("framebuffer size: width=%d, height=%d\n", framebuffer_width, framebuffer_height);


    glfwMakeContextCurrent(widget_window);

    glfwSetWindowSizeCallback(widget_window, window_size_callback);
    glfwSetFramebufferSizeCallback(widget_window, framebuffer_size_callback);

    // 初始化窗口大小 (逻辑分辨率）
    int widget_window_width, widget_window_height;
    glfwGetWindowSize(widget_window, &widget_window_width, &widget_window_height);
    window_size_callback(widget_window, widget_window_width, widget_window_height);

    // 初始化 viewport (物理分辨率）
    glfwGetFramebufferSize(widget_window, &framebuffer_width, &framebuffer_height);
    framebuffer_size_callback(widget_window, framebuffer_width, framebuffer_height);

    glfwMakeContextCurrent(window);

    cv::Mat cursor_image = create_cursor_image();

    // 创建 GLFW 光标
    GLFWimage glfwCursorImage;
    glfwCursorImage.width = cursor_image.cols;
    glfwCursorImage.height = cursor_image.rows;
    glfwCursorImage.pixels = cursor_image.data;
    GLFWcursor* customCursor = glfwCreateCursor(&glfwCursorImage, 0, 0);

    // 设置窗口光标
    glfwSetCursor(window, customCursor);

    // 获取图像


    //printf("image size: width=%d, height=%d\n", image_width, image_height);

//    cv::Size dsize;
//    dsize.width = image.cols / (framebuffer_width / window_width);
//    dsize.height = image.rows / (framebuffer_height / window_height);
//    cv::resize(image, image, dsize, 0, 0, cv::INTER_AREA);

    const int image_width = image.cols;
    const int image_height = image.rows;

    // 生成纹理
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 上传纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    glBindTexture(GL_TEXTURE_2D, 0);

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        if (confirmed)
            break;

        if (selected)
        {
            glfwShowWindow(widget_window);
            //glfwFocusWindow(widget_window); // 将焦点设置到工具箱窗口
            glfwMakeContextCurrent(widget_window);

            //glViewport(0, 0, 400, 140);

            // 在工具箱窗口中渲染内容
            //render_toolbox_window(widget_window, buttons);
            glClear(GL_COLOR_BUFFER_BIT);

            drawX(buttons[0]);
            drawCheckMark(buttons[1]);
            glfwSwapBuffers(widget_window);

            int xpos = std::max(startX, currentX) - 200;
            int ypos = std::max(startY, currentY) + 10;
            glfwSetWindowPos(widget_window, xpos, ypos);
        }
        else
        {
            glfwHideWindow(widget_window);
        }

        glfwMakeContextCurrent(window);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw_textured_quad(texture, image_width, image_height);
        draw_textured_quad(texture, window_width, window_height);
        draw_overlay(startX, startY, currentX, currentY, window_width, window_height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理
    glfwDestroyWindow(window);
    glfwDestroyWindow(widget_window);
    glfwTerminate();
    return 0;
}
