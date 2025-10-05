#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include "heatmap.h"
#include "metrics.h"

namespace VideoQuality {

class Dashboard {
public:
    Dashboard(const std::string& originalPath, 
              const std::string& compressedPath);
    ~Dashboard();
    
    // Run the interactive dashboard
    void run();
    
    // Window names
    static const std::string WIN_ORIGINAL;
    static const std::string WIN_COMPRESSED;
    static const std::string WIN_HEATMAP;
    static const std::string WIN_CONTROLS;
    
private:
    // Video captures
    cv::VideoCapture originalVideo_;
    cv::VideoCapture compressedVideo_;
    
    // Current state
    int currentFrame_;
    int totalFrames_;
    double fps_;
    bool playing_;
    
    // Modules
    HeatmapGenerator heatmapGen_;
    
    // Metrics storage
    struct FrameMetrics {
        double psnr;
        double ssim;
    };
    std::vector<FrameMetrics> metricsCache_;
    
    // UI methods
    void setupWindows();
    void updateDisplay();
    void drawControlPanel(cv::Mat& panel);
    void drawTimeline(cv::Mat& panel, int yPos);
    void drawMetrics(cv::Mat& panel, int yPos);
    
    // Video control
    void seekToFrame(int frameNum);
    void nextFrame();
    void prevFrame();
    void togglePlayback();
    
    // Metrics calculation
    void calculateMetricsForFrame(int frameNum);
    void precalculateMetrics();
    
    // Keyboard handling
    void handleKeyPress(int key);
    
    // Trackbar callback
    static void onTrackbar(int pos, void* userdata);
    
    // Display settings
    int displayWidth_;
    int displayHeight_;
    double heatmapAlpha_;
    int colormapType_;
};

} // namespace VideoQuality

#endif // DASHBOARD_H