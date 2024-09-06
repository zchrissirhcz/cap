#include <Cocoa/Cocoa.h>
#include "transparent_window.h"

// Define a delegate class to handle window events
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@implementation AppDelegate
- (void)windowWillClose:(NSNotification *)notification {
    // Terminate the application when the window is closed
    [NSApp terminate:nil];
}
@end

void showObjectiveCWindow() {
    @autoreleasepool {
        // Create an application instance
        [NSApplication sharedApplication];

        // Create a window
        NSRect frame = NSMakeRect(100, 100, 400, 300);
        NSWindow *window = [[NSWindow alloc] initWithContentRect:frame
                                                       styleMask:(NSWindowStyleMaskTitled |
                                                                  NSWindowStyleMaskClosable |
                                                                  NSWindowStyleMaskResizable)
                                                         backing:NSBackingStoreBuffered
                                                           defer:NO];

        // Set the window title
        [window setTitle:@"Objective-C++ Window"];

        // Create and set the delegate
        AppDelegate *delegate = [[AppDelegate alloc] init];
        [NSApp setDelegate:delegate];
        [window setDelegate:delegate];

        // Make the window visible
        [window makeKeyAndOrderFront:nil];

        // Run the application
        [NSApp run];
    }
}

void closeObjectiveCWindow() {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSApp terminate:nil];
    });
}