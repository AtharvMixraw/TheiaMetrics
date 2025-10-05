#include <iostream>
#include <string>
#include "dashboard.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Video Quality Dashboard" << std::endl;
        std::cout << "Usage: " << argv[0] << " <original_video> <compressed_video>" << std::endl;
        std::cout << std::endl;
        std::cout << "Example:" << std::endl;
        std::cout << "  " << argv[0] << " data/input.mp4 data/compressed/output_500k.mp4" << std::endl;
        return -1;
    }

    std::string originalPath = argv[1];
    std::string compressedPath = argv[2];

    try {
        std::cout << "Initializing Video Quality Dashboard..." << std::endl;
        std::cout << "Original: " << originalPath << std::endl;
        std::cout << "Compressed: " << compressedPath << std::endl;
        std::cout << std::endl;

        VideoQuality::Dashboard dashboard(originalPath, compressedPath);
        dashboard.run();

        std::cout << "Dashboard closed." << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
}