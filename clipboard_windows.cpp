#include "clipboard.hpp"  

#define WIN32_LEAN_AND_MEAN // 排除不常用的 API  
#define NOMINMAX            // 防止定义 min 和 max 宏  
//#define NOGDI               // 排除 GDI 功能  
//#define NOSERVICE           // 排除服务功能  
//#define NOIME               // 排除输入法编辑器功能  
//#define NOMCX               // 排除 Modem Control Extensions  
//#define NOCRYPT             // 排除加密/解密功能  

#include <windows.h>  
#include <opencv2/opencv.hpp>  


void copyImageToClipboard(const cv::Mat& image) {
    // Convert image to BGRA format  
    cv::Mat convertedImage;
    cv::cvtColor(image, convertedImage, cv::COLOR_BGR2BGRA);

    // Calculate the size of the image data  
    const int dataSize = convertedImage.total() * convertedImage.elemSize();

    // Open the clipboard  
    if (!OpenClipboard(nullptr)) {
        return;
    }

    // Empty the clipboard  
    EmptyClipboard();

    // Allocate global memory for the image data  
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, dataSize + sizeof(BITMAPINFOHEADER));
    if (!hMem) {
        CloseClipboard();
        return;
    }

    // Lock the global memory and copy the image data  
    void* pMem = GlobalLock(hMem);
    if (pMem) {
        // Prepare the BITMAPINFOHEADER  
        BITMAPINFOHEADER* bi = static_cast<BITMAPINFOHEADER*>(pMem);
        bi->biSize = sizeof(BITMAPINFOHEADER);
        bi->biWidth = convertedImage.cols;
        bi->biHeight = -convertedImage.rows; // Negative to indicate top-down DIB  
        bi->biPlanes = 1;
        bi->biBitCount = 32;
        bi->biCompression = BI_RGB;
        bi->biSizeImage = 0;
        bi->biXPelsPerMeter = 0;
        bi->biYPelsPerMeter = 0;
        bi->biClrUsed = 0;
        bi->biClrImportant = 0;

        // Copy the actual image data after the header  
        memcpy(static_cast<char*>(pMem) + sizeof(BITMAPINFOHEADER), convertedImage.data, dataSize);
    }
    GlobalUnlock(hMem);

    // Set the clipboard data  
    SetClipboardData(CF_DIB, hMem);

    // Close the clipboard  
    CloseClipboard();

    // The clipboard now owns the memory, so we don't free it  
}