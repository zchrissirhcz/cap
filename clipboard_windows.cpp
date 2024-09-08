#include "clipboard.hpp"  

#include <opencv2/opencv.hpp>  
#include <Windows.h>  
#include <iostream>  

// Helper function to create a HBITMAP from a cv::Mat  
HBITMAP CreateHBITMAPFromCvMat(const cv::Mat& image)  
{  
    cv::Mat convertedImage;  
    if (image.channels() == 1) {  
        cv::cvtColor(image, convertedImage, cv::COLOR_GRAY2BGRA);  
    } else if (image.channels() == 3) {  
        cv::cvtColor(image, convertedImage, cv::COLOR_BGR2BGRA);  
    } else if (image.channels() == 4) {  
        convertedImage = image.clone();  
    } else {  
        std::cerr << "Unsupported image format." << std::endl;  
        return nullptr;  
    }  

    BITMAPINFOHEADER bi = { 0 };  
    bi.biSize = sizeof(BITMAPINFOHEADER);  
    bi.biWidth = convertedImage.cols;  
    bi.biHeight = -convertedImage.rows; // Negative to indicate a top-down DIB  
    bi.biPlanes = 1;  
    bi.biBitCount = 32;  
    bi.biCompression = BI_RGB;  

    HBITMAP hBitmap = CreateDIBSection(nullptr, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS, nullptr, nullptr, 0);  
    if (!hBitmap) {  
        std::cerr << "Failed to create a DIB section." << std::endl;  
        return nullptr;  
    }  

    void* pvImageBits = nullptr;  
    GetDIBits(GetDC(nullptr), hBitmap, 0, convertedImage.rows, NULL, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS);  
    pvImageBits = convertedImage.data;  
    SetDIBits(GetDC(nullptr), hBitmap, 0, convertedImage.rows, pvImageBits, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS);  

    return hBitmap;  
}  

// Copy cv::Mat image to system clipboard  
void copyImageToClipboard(const cv::Mat& image)  
{  
    // Check if image is empty  
    if (image.empty()) {  
        std::cerr << "Image is empty, cannot copy to clipboard." << std::endl;  
        return;  
    }  

    HBITMAP hBitmap = CreateHBITMAPFromCvMat(image);  
    if (!hBitmap) {  
        std::cerr << "Failed to create HBITMAP from cv::Mat." << std::endl;  
        return;  
    }  

    // Open the clipboard and clear any previous contents  
    if (OpenClipboard(nullptr)) {  
        EmptyClipboard();  
        SetClipboardData(CF_BITMAP, hBitmap);  
        CloseClipboard();  
    } else {  
        std::cerr << "Failed to open clipboard." << std::endl;  
    }  

    // The clipboard takes ownership of the HBITMAP, do not delete it  
}  