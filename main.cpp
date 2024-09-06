#include <ApplicationServices/ApplicationServices.h>  
#include <iostream>  

int main() {  
    // Get the main display ID  
    CGDirectDisplayID displayId = CGMainDisplayID();  

    // Capture the screenshot  
    CGImageRef screenshot = CGDisplayCreateImage(displayId);  
    if (!screenshot) {  
        std::cerr << "Failed to capture screenshot." << std::endl;  
        return -1;  
    }  

    // Create a URL for the file  
    CFURLRef url = CFURLCreateWithFileSystemPath(  
        kCFAllocatorDefault,  
        CFSTR("screenshot.png"),  
        kCFURLPOSIXPathStyle,  
        false  
    );  

    // Create a destination for the image  
    CGImageDestinationRef destination = CGImageDestinationCreateWithURL(  
        url,  
        kUTTypePNG,  
        1,  
        nullptr  
    );  

    if (!destination) {  
        std::cerr << "Failed to create image destination." << std::endl;  
        CFRelease(screenshot);  
        CFRelease(url);  
        return -1;  
    }  

    // Add the image to the destination  
    CGImageDestinationAddImage(destination, screenshot, nullptr);  

    // Finalize the image destination  
    if (!CGImageDestinationFinalize(destination)) {  
        std::cerr << "Failed to write image to file." << std::endl;  
    } else {  
        std::cout << "Screenshot saved successfully!" << std::endl;  
    }  

    // Clean up  
    CFRelease(destination);  
    CFRelease(screenshot);  
    CFRelease(url);  

    return 0;  
}  
