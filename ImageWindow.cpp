//------------------------------------------------------------------------------
// File:   ImageWindow.cpp
// Author: PSCD
// Date:   septiembre 2026
// Coms:   Implementa ImageWindow.hpp
//------------------------------------------------------------------------------
#include <iostream>
#include <stdexcept>
#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

#include "ImageWindow.hpp"
//------------------------------------------------------
ImageWindow::ImageWindow(const std::string& path2Viewer) {
    std::string cmd = PYTHON + DIRSEP + path2Viewer + DIRSEP + VIEWER;

    py = popen(cmd.c_str(), "w");

    if (!py) {
        throw std::runtime_error("Cannot start " + VIEWER);
    }
}

ImageWindow::ImageWindow(const std::string& path2Viewer, int width, int height)
{
    std::string cmd =
        PYTHON + " " + path2Viewer + DIRSEP + VIEWER + " "
        + std::to_string(width) + " "
        + std::to_string(height);
    
    py = popen(cmd.c_str(), "w");

    if (!py) {
        throw std::runtime_error("Cannot start " + VIEWER);
    }
}

ImageWindow::ImageWindow(const std::string& path2Viewer,
                        int width, int height, int X, int Y)
{
    std::string cmd =
        PYTHON + " " + path2Viewer + DIRSEP + VIEWER + " "
        + std::to_string(width) + " "
        + std::to_string(height) + " "
        + std::to_string(X) + " "
        + std::to_string(Y);
    
    py = popen(cmd.c_str(), "w");

    if (!py) {
        throw std::runtime_error("Cannot start " + VIEWER);
    }
}
//------------------------------------------------------
ImageWindow::~ImageWindow() {
    close();
}
//------------------------------------------------------
void ImageWindow::show(const std::string& imageFileName) {
    if (!py) {
        std::cerr << "Cannot show image\n";
    }
    else {
        fprintf(py, "%s\n", imageFileName.c_str());
        fflush(py);
        // std::cout << imageFileName << std::endl;
    }
}
//------------------------------------------------------
void ImageWindow::close() {
    if (py) {
        fprintf(py, "quit\n");
        fflush(py);

        pclose(py);
        py = nullptr;
    }
}
//------------------------------------------------------