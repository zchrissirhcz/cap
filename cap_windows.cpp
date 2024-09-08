#include "cap.h"

#include <Windows.h>
#include <iostream>

void ConvertBGRAToRGBA(unsigned char* buffer, int width, int height) {  
    for (int i = 0; i < width * height; ++i) {  
        unsigned char* pixel = buffer + i * 4;  
        std::swap(pixel[0], pixel[2]); // 交换蓝色和红色通道  
    }  
}

unsigned char* captureFullScreen(int* width, int* height, int* channels) {
    // 获取屏幕设备上下文
    HDC hScreenDC = GetDC(nullptr);
    if (!hScreenDC) {
        std::cerr << "Failed to get screen DC" << std::endl;
        return nullptr;
    }
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    if (!hMemoryDC) {
        std::cerr << "Failed to create compatible DC" << std::endl;
        ReleaseDC(nullptr, hScreenDC);
        return nullptr;
    }

    // 获取屏幕尺寸
    *width = GetDeviceCaps(hScreenDC, HORZRES);
    *height = GetDeviceCaps(hScreenDC, VERTRES);
    *channels = 4; // 使用 RGBA

    // 创建屏幕兼容位图
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, *width, *height);
    if (!hBitmap) {
        std::cerr << "Failed to create bitmap" << std::endl;
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);
        return nullptr;
    }

    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

    // 执行位块传递操作
    if (!BitBlt(hMemoryDC, 0, 0, *width, *height, hScreenDC, 0, 0, SRCCOPY)) {
        std::cerr << "BitBlt failed" << std::endl;

        // 释放资源
        SelectObject(hMemoryDC, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);
        return nullptr;
    }

    // 获取BITMAP信息
    BITMAP bmpScreen;
    GetObject(hBitmap, sizeof(BITMAP), &bmpScreen);

    // 创建缓冲区
    unsigned char* buffer = new unsigned char[bmpScreen.bmWidthBytes * bmpScreen.bmHeight];

    // 设置位图信息头
    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = *width;
    bi.biHeight = -*height; // 上下翻转
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    // 获取位图数据
    if (!GetDIBits(hMemoryDC, hBitmap, 0, (UINT)*height, buffer, (BITMAPINFO*)&bi, DIB_RGB_COLORS)) {
        std::cerr << "GetDIBits failed" << std::endl;

        // 释放缓冲区
        delete[] buffer;
        buffer = nullptr;
    }

    // 清理
    SelectObject(hMemoryDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(nullptr, hScreenDC);
    
    ConvertBGRAToRGBA(buffer, *width, *height);

    return buffer;
}