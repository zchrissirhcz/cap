#include "cap.h"

#include <ApplicationServices/ApplicationServices.h>
#include <iostream>

unsigned char* captureFullScreen(int* width, int* height, int* channels) {
    // Capture the screen
    CGImageRef screenImageRef = CGDisplayCreateImage(kCGDirectMainDisplay);
    if (!screenImageRef) {
        std::cerr << "Failed to capture screen" << std::endl;
        return nullptr;
    }

    // Get image properties
    *width = static_cast<int>(CGImageGetWidth(screenImageRef));
    *height = static_cast<int>(CGImageGetHeight(screenImageRef));
    *channels = 4; // Assuming RGBA

    // Create a bitmap context
    CGContextRef context = CGBitmapContextCreate(NULL, *width, *height, 8, *width * *channels, CGImageGetColorSpace(screenImageRef), kCGImageAlphaPremultipliedLast);
    if (!context) {
        std::cerr << "Failed to create bitmap context" << std::endl;
        CFRelease(screenImageRef);
        return nullptr;
    }

    // Draw the image into the context
    CGContextDrawImage(context, CGRectMake(0, 0, *width, *height), screenImageRef);

    // Get the buffer
    unsigned char* buffer = (unsigned char*)CGBitmapContextGetData(context);

    // Retain the buffer and release the context
    unsigned char* bufferCopy = (unsigned char*)malloc(*width * *height * *channels);
    memcpy(bufferCopy, buffer, *width * *height * *channels);
    CGContextRelease(context);
    CFRelease(screenImageRef);

    return bufferCopy;
}