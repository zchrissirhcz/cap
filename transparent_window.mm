#include <Cocoa/Cocoa.h>
#include "transparent_window.h"

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@implementation AppDelegate
- (void)windowWillClose:(NSNotification *)notification {
    [NSApp terminate:nil];
}
@end

void captureScreen() {
    CGImageRef screenImage = CGDisplayCreateImage(kCGDirectMainDisplay);
    if (screenImage) {
        NSBitmapImageRep *bitmapRep = [[NSBitmapImageRep alloc] initWithCGImage:screenImage];
        NSData *pngData = [bitmapRep representationUsingType:NSBitmapImageFileTypePNG properties:@{}];
        NSString *filePath = [[NSFileManager defaultManager] currentDirectoryPath];
        filePath = [filePath stringByAppendingPathComponent:@"screenshot.png"];
        [pngData writeToFile:filePath atomically:YES];
        CFRelease(screenImage);
    }
}

void showTransparentWindow() {
    @autoreleasepool {
        [NSApplication sharedApplication];

        NSRect frame = NSMakeRect(100, 100, 400, 300);
        NSWindow *window = [[NSWindow alloc] initWithContentRect:frame
                                                       styleMask:(NSWindowStyleMaskTitled |
                                                                  NSWindowStyleMaskClosable |
                                                                  NSWindowStyleMaskResizable)
                                                         backing:NSBackingStoreBuffered
                                                           defer:NO];
        [window setBackgroundColor:[NSColor whiteColor]]; // Set the background color to white
        [window setAlphaValue:0.7]; // Set window transparency to 70%
        [window setOpaque:NO]; // Ensure the window is not opaque
        [window setTitle:@"Semi-Transparent White Window"];

        AppDelegate *delegate = [[AppDelegate alloc] init];
        [NSApp setDelegate:delegate];
        [window setDelegate:delegate];

        [window makeKeyAndOrderFront:nil];

        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            captureScreen();
            [window close];
        });

        [NSApp run];
    }
}