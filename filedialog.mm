#include "filedialog.hpp"
#include <Cocoa/Cocoa.h>  

// 显示保存对话框并返回用户选择的文件路径  
std::string showSaveFileDialog() {  
    // 创建保存面板  
    NSSavePanel* savePanel = [NSSavePanel savePanel];  

    // 设置保存面板的属性  
    [savePanel setTitle:@"Save File"];  
    [savePanel setAllowedFileTypes:@[@"png", @"jpg", @"jpeg", @"bmp", @"tiff"]]; // 允许的文件类型  

    // 显示保存面板并获取用户选择的结果  
    if ([savePanel runModal] == NSModalResponseOK) {  
        NSURL* fileURL = [savePanel URL];  
        if (fileURL) {  
            return std::string([[fileURL path] UTF8String]);  
        }  
    }  

    return ""; // 如果用户取消，则返回空字符串  
}