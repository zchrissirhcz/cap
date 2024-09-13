#include "filedialog.hpp"
#include "portable-file-dialogs.h"

// Show save file dialog and return the selected file path, or an empty string if the user cancels
std::string showSaveFileDialog() {
    // Create a save file dialog
    auto save = pfd::save_file("Save Image",
                               "untitled.png", // default file name
                               { "Image Files", "*.png *.jpg *.jpeg *.bmp *.tiff", "All Files", "*" },
                               pfd::opt::force_overwrite);

    std::string filePath = save.result();

    return filePath.empty() ? "" : filePath;
}