#include "dashboard.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace VideoQuality {

const std::string Dashboard::WIN_ORIGINAL = "Original Video";
const std::string Dashboard::WIN_COMPRESSED = "Compressed Video";
const std::string Dashboard::WIN_HEATMAP = "Difference Heatmap";
const std::string Dashboard::WIN_CONTROLS = "Controls & Metrics";

Dashboard::Dashboard(const std::string& originalPath,
                     const std::string& compressedPath)
    : currentFrame_(0), playing_(false), displayWidth_(640), 
      displayHeight_(360), heatmapAlpha_(0.5), 
      colormapType_(cv::COLORMAP_JET) {
    
    // Open videos
    originalVideo_.open(originalPath);
    compressedVideo_.open(compressedPath);
    
    if (!originalVideo_.isOpened() || !compressedVideo_.isOpened()) {
        throw std::runtime_error("Failed to open video files");
    }
    
    // Get video properties
    totalFrames_ = static_cast<int>(originalVideo_.get(cv::CAP_PROP_FRAME_COUNT));
    fps_ = originalVideo_.get(cv::CAP_PROP_FPS);
    
    std::cout << "Loaded videos: " << totalFrames_ << " frames at " 
              << fps_ << " fps" << std::endl;
    
    // Initialize metrics cache
    metricsCache_.resize(totalFrames_);
}

Dashboard::~Dashboard() {
    cv::destroyAllWindows();
}

void Dashboard::setupWindows() {
    cv::namedWindow(WIN_ORIGINAL, cv::WINDOW_AUTOSIZE);
    cv::namedWindow(WIN_COMPRESSED, cv::WINDOW_AUTOSIZE);
    cv::namedWindow(WIN_HEATMAP, cv::WINDOW_AUTOSIZE);
    cv::namedWindow(WIN_CONTROLS, cv::WINDOW_AUTOSIZE);
    
    // Create trackbar for frame navigation
    cv::createTrackbar("Frame", WIN_CONTROLS, &currentFrame_, 
                      totalFrames_ - 1, onTrackbar, this);
}

void Dashboard::onTrackbar(int pos, void* userdata) {
    Dashboard* dash = static_cast<Dashboard*>(userdata);
    dash->seekToFrame(pos);
}

void Dashboard::seekToFrame(int frameNum) {
    if (frameNum < 0 || frameNum >= totalFrames_) return;
    
    currentFrame_ = frameNum;
    originalVideo_.set(cv::CAP_PROP_POS_FRAMES, currentFrame_);
    compressedVideo_.set(cv::CAP_PROP_POS_FRAMES, currentFrame_);
    
    updateDisplay();
}

void Dashboard::nextFrame() {
    if (currentFrame_ < totalFrames_ - 1) {
        seekToFrame(currentFrame_ + 1);
    }
}

void Dashboard::prevFrame() {
    if (currentFrame_ > 0) {
        seekToFrame(currentFrame_ - 1);
    }
}

void Dashboard::togglePlayback() {
    playing_ = !playing_;
}

void Dashboard::calculateMetricsForFrame(int frameNum) {
    if (frameNum < 0 || frameNum >= totalFrames_) return;
    
    // Seek to frame
    originalVideo_.set(cv::CAP_PROP_POS_FRAMES, frameNum);
    compressedVideo_.set(cv::CAP_PROP_POS_FRAMES, frameNum);
    
    cv::Mat origFrame, compFrame;
    originalVideo_.read(origFrame);
    compressedVideo_.read(compFrame);
    
    if (!origFrame.empty() && !compFrame.empty()) {
        // Resize if needed
        if (origFrame.size() != compFrame.size()) {
            cv::resize(compFrame, compFrame, origFrame.size());
        }
        
        // Calculate metrics
        metricsCache_[frameNum].psnr = getPSNR(origFrame, compFrame);
        metricsCache_[frameNum].ssim = getMSSIM(origFrame, compFrame)[0];
    }
}

void Dashboard::precalculateMetrics() {
    std::cout << "Pre-calculating metrics for all frames..." << std::endl;
    
    for (int i = 0; i < totalFrames_; ++i) {
        calculateMetricsForFrame(i);
        
        if (i % 100 == 0) {
            std::cout << "Progress: " << i << "/" << totalFrames_ << std::endl;
        }
    }
    
    std::cout << "Metrics calculation complete." << std::endl;
}

void Dashboard::drawMetrics(cv::Mat& panel, int yPos) {
    if (currentFrame_ >= metricsCache_.size()) return;
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "PSNR: " << metricsCache_[currentFrame_].psnr << " dB";
    
    cv::putText(panel, oss.str(), cv::Point(10, yPos),
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    
    oss.str("");
    oss << "SSIM: " << metricsCache_[currentFrame_].ssim;
    cv::putText(panel, oss.str(), cv::Point(10, yPos + 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
}

void Dashboard::drawTimeline(cv::Mat& panel, int yPos) {
    int timelineWidth = panel.cols - 20;
    int timelineHeight = 20;
    cv::Point startPt(10, yPos);
    cv::Point endPt(10 + timelineWidth, yPos);
    
    // Draw timeline bar
    cv::rectangle(panel, startPt, cv::Point(endPt.x, endPt.y + timelineHeight),
                 cv::Scalar(100, 100, 100), -1);
    
    // Draw current position
    int posX = startPt.x + (timelineWidth * currentFrame_ / totalFrames_);
    cv::rectangle(panel, cv::Point(posX - 2, yPos),
                 cv::Point(posX + 2, yPos + timelineHeight),
                 cv::Scalar(0, 255, 0), -1);
    
    // Draw frame number
    std::ostringstream oss;
    oss << "Frame: " << currentFrame_ + 1 << " / " << totalFrames_;
    cv::putText(panel, oss.str(), cv::Point(10, yPos + timelineHeight + 25),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
}

void Dashboard::drawControlPanel(cv::Mat& panel) {
    panel = cv::Mat::zeros(300, 640, CV_8UC3);
    
    // Title
    cv::putText(panel, "Video Quality Dashboard", cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 2);
    
    // Controls info
    std::vector<std::string> controls = {
        "SPACE: Play/Pause",
        "RIGHT: Next Frame",
        "LEFT: Previous Frame",
        "Q/ESC: Quit",
        "H: Toggle Heatmap Overlay",
        "C: Change Colormap"
    };
    
    int yPos = 70;
    for (const auto& ctrl : controls) {
        cv::putText(panel, ctrl, cv::Point(10, yPos),
                   cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(200, 200, 200), 1);
        yPos += 20;
    }
    
    // Metrics
    drawMetrics(panel, 200);
    
    // Timeline
    drawTimeline(panel, 250);
}

void Dashboard::updateDisplay() {
    cv::Mat origFrame, compFrame;
    
    // Read current frames
    originalVideo_.set(cv::CAP_PROP_POS_FRAMES, currentFrame_);
    compressedVideo_.set(cv::CAP_PROP_POS_FRAMES, currentFrame_);
    
    originalVideo_.read(origFrame);
    compressedVideo_.read(compFrame);
    
    if (origFrame.empty() || compFrame.empty()) return;
    
    // Resize for display
    cv::Mat origDisplay, compDisplay;
    cv::resize(origFrame, origDisplay, cv::Size(displayWidth_, displayHeight_));
    cv::resize(compFrame, compDisplay, cv::Size(displayWidth_, displayHeight_));
    
    // Generate heatmap
    cv::Mat heatmap = heatmapGen_.generateOverlay(origFrame, compFrame, 
                                                   heatmapAlpha_, colormapType_);
    cv::Mat heatmapDisplay;
    cv::resize(heatmap, heatmapDisplay, cv::Size(displayWidth_, displayHeight_));
    
    // Draw labels on frames
    cv::putText(origDisplay, "Original", cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
    cv::putText(compDisplay, "Compressed", cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
    cv::putText(heatmapDisplay, "Difference", cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
    
    // Display
    cv::imshow(WIN_ORIGINAL, origDisplay);
    cv::imshow(WIN_COMPRESSED, compDisplay);
    cv::imshow(WIN_HEATMAP, heatmapDisplay);
    
    // Update control panel
    cv::Mat controlPanel;
    drawControlPanel(controlPanel);
    cv::imshow(WIN_CONTROLS, controlPanel);
}

void Dashboard::handleKeyPress(int key) {
    switch (key) {
        case 32: // SPACE
            togglePlayback();
            break;
        case 81: // Right arrow
        case 83:
            nextFrame();
            break;
        case 82: // Left arrow
        case 84:
            prevFrame();
            break;
        case 'q':
        case 27: // ESC
            playing_ = false;
            cv::destroyAllWindows();
            break;
        case 'h':
        case 'H':
            heatmapAlpha_ = (heatmapAlpha_ == 0.5) ? 0.7 : 0.5;
            updateDisplay();
            break;
        case 'c':
        case 'C':
            colormapType_ = (colormapType_ == cv::COLORMAP_JET) ? 
                           cv::COLORMAP_HOT : cv::COLORMAP_JET;
            updateDisplay();
            break;
    }
}

void Dashboard::run() {
    setupWindows();
    
    std::cout << "Calculating metrics..." << std::endl;
    precalculateMetrics();
    
    std::cout << "Starting dashboard..." << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  SPACE: Play/Pause" << std::endl;
    std::cout << "  Arrow Keys: Navigate frames" << std::endl;
    std::cout << "  Q/ESC: Quit" << std::endl;
    
    seekToFrame(0);
    
    // Main loop
    while (true) {
        if (playing_) {
            nextFrame();
            if (currentFrame_ >= totalFrames_ - 1) {
                playing_ = false;
            }
        }
        
        int key = cv::waitKey(playing_ ? static_cast<int>(1000.0 / fps_) : 30);
        if (key != -1) {
            handleKeyPress(key);
            if (key == 'q' || key == 'Q' || key == 27) {
                break;
            }
        }
    }
}

} // namespace VideoQuality