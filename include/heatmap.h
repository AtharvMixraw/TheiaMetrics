#ifndef HEATMAP_H
#define HEATMAP_H

#include <opencv2/opencv.hpp>

namespace VideoQuality {

class HeatmapGenerator {
public:
    HeatmapGenerator();
    
    // Generate error heatmap from two frames
    cv::Mat generateHeatmap(const cv::Mat& original, 
                           const cv::Mat& compressed,
                           int colormapType = cv::COLORMAP_JET);
    
    // Generate with overlay on original
    cv::Mat generateOverlay(const cv::Mat& original,
                           const cv::Mat& compressed,
                           double alpha = 0.5,
                           int colormapType = cv::COLORMAP_JET);
    
    // Calculate difference statistics
    struct DifferenceStats {
        double minError;
        double maxError;
        double meanError;
        double stdError;
    };
    
    DifferenceStats calculateStats(const cv::Mat& original,
                                   const cv::Mat& compressed);
    
    // Set sensitivity threshold (0-255)
    void setThreshold(double threshold);
    
private:
    double threshold_;
    cv::Mat calculateDifference(const cv::Mat& img1, const cv::Mat& img2);
    cv::Mat normalizeForDisplay(const cv::Mat& diff);
};

} // namespace VideoQuality

#endif // HEATMAP_H