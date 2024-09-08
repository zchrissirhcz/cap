#include "clipboard.hpp"

#include <opencv2/opencv.hpp>
#include <Cocoa/Cocoa.h>

// 将cv::Mat图像复制到系统剪贴板
void copyImageToClipboard(const cv::Mat& image)
{
    // 检查图像是否为空
    if (image.empty()) {
        std::cerr << "Image is empty, cannot copy to clipboard." << std::endl;
        return;
    }

    // 将cv::Mat转换为NSImage
    cv::Mat convertedImage;
    if (image.channels() == 1) {
        cv::cvtColor(image, convertedImage, cv::COLOR_GRAY2RGBA);
    } else if (image.channels() == 3) {
        cv::cvtColor(image, convertedImage, cv::COLOR_BGR2RGBA);
    } else if (image.channels() == 4) {
        convertedImage = image.clone();
    } else {
        std::cerr << "Unsupported image format." << std::endl;
        return;
    }

    NSBitmapImageRep* imgRep = [[NSBitmapImageRep alloc]
        initWithBitmapDataPlanes:NULL
                      pixelsWide:convertedImage.cols
                      pixelsHigh:convertedImage.rows
                   bitsPerSample:8
                 samplesPerPixel:4
                        hasAlpha:YES
                        isPlanar:NO
                  colorSpaceName:NSDeviceRGBColorSpace
                    bitmapFormat:NSAlphaNonpremultipliedBitmapFormat
                     bytesPerRow:convertedImage.step
                    bitsPerPixel:32];

    memcpy([imgRep bitmapData], convertedImage.data, convertedImage.total() * convertedImage.elemSize());

    NSImage* nsImage = [[NSImage alloc] initWithSize:NSMakeSize(convertedImage.cols, convertedImage.rows)];
    [nsImage addRepresentation:imgRep];

    // 将NSImage复制到剪贴板
    NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
    [pasteboard clearContents];
    [pasteboard writeObjects:@[nsImage]];

    // 释放对象
    [nsImage release];
    [imgRep release];
}