#include <Cocoa/Cocoa.h>
#include "cap.h"

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@implementation AppDelegate
- (void)windowWillClose:(NSNotification *)notification {
    [NSApp terminate:nil];
}
@end

@interface CaptureWindow : NSWindow
@property (assign) NSPoint startPoint;
@property (assign) NSPoint endPoint;
@end

@implementation CaptureWindow

- (void)mouseDown:(NSEvent *)event {
    self.startPoint = [event locationInWindow];
}

- (void)mouseUp:(NSEvent *)event {
    self.endPoint = [event locationInWindow];
    [self captureScreen];
    [self close];
}

- (void)captureScreen {
    CGFloat x = MIN(self.startPoint.x, self.endPoint.x);
    CGFloat y = MIN(self.startPoint.y, self.endPoint.y);
    CGFloat width = fabs(self.startPoint.x - self.endPoint.x);
    CGFloat height = fabs(self.startPoint.y - self.endPoint.y);
    CGRect captureRect = CGRectMake(x, y, width, height);

    CGImageRef screenImage = CGDisplayCreateImageForRect(kCGDirectMainDisplay, captureRect);
    if (screenImage) {
        NSBitmapImageRep *bitmapRep = [[NSBitmapImageRep alloc] initWithCGImage:screenImage];
        NSData *pngData = [bitmapRep representationUsingType:NSBitmapImageFileTypePNG properties:@{}];
        NSString *filePath = [[NSFileManager defaultManager] currentDirectoryPath];
        filePath = [filePath stringByAppendingPathComponent:@"capture.png"];
        [pngData writeToFile:filePath atomically:YES];
        CFRelease(screenImage);
    }
}

@end

void showTransparentWindow() {
    @autoreleasepool {
        [NSApplication sharedApplication];

        NSScreen *mainScreen = [NSScreen mainScreen];
        NSRect frame = [mainScreen frame];

        CaptureWindow *window = [[CaptureWindow alloc] initWithContentRect:frame
                                                                 styleMask:(NSWindowStyleMaskBorderless)
                                                                   backing:NSBackingStoreBuffered
                                                                     defer:NO];
        [window setBackgroundColor:[NSColor colorWithWhite:1.0 alpha:0.7]];
        [window setOpaque:NO];
        [window setLevel:NSStatusWindowLevel];

        AppDelegate *delegate = [[AppDelegate alloc] init];
        [NSApp setDelegate:delegate];
        [window setDelegate:delegate];

        [window makeKeyAndOrderFront:nil];
        [NSApp run];
    }
}