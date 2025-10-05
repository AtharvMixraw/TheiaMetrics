#include "heatmap.h"
#include <iostream>

namespace VideoQuality {

HeatmapGenerator::HeatmapGenerator() : threshold_(10.0) {}

cv::Mat HeatmapGenerator::calculateDifference(const cv::Mat& img1, 
                                              const cv::Mat& img2) {
    cv::Mat diff;
    cv::absdiff(img1, img2, diff);
    
    // Convert to grayscale if color
    if (diff.channels() == 3) {
        cv::cvtColor(diff, diff, cv::COLOR_BGR2GRAY);
    }
    
    return diff;
}

cv::Mat HeatmapGenerator::normalizeForDisplay(const cv::Mat& diff) {
    cv::Mat normalized;
    double minVal, maxVal;
    cv::minMaxLoc(diff, &minVal, &maxVal);
    
    if (maxVal > 0) {
        diff.convertTo(normalized, CV_8U, 255.0 / maxVal);
    } else {
        normalized = cv::Mat::zeros(diff.size(), CV_8U);
    }
    
    return normalized;
}

cv::Mat HeatmapGenerator::generateHeatmap(const cv::Mat& original,
                                          const cv::Mat& compressed,
                                          int colormapType) {
    // Ensure same size
    cv::Mat comp_resized;
    if (original.size() != compressed.size()) {
        cv::resize(compressed, comp_resized, original.size());
    } else {
        comp_resized = compressed.clone();
    }
    
    // Calculate difference
    cv::Mat diff = calculateDifference(original, comp_resized);
    
    // Apply threshold
    cv::Mat thresholded;
    cv::threshold(diff, thresholded, threshold_, 255, cv::THRESH_TOZERO);
    
    // Normalize for display
    cv::Mat normalized = normalizeForDisplay(thresholded);
    
    // Apply colormap
    cv::Mat heatmap;
    cv::applyColorMap(normalized, heatmap, colormapType);
    
    return heatmap;
}

cv::Mat HeatmapGenerator::generateOverlay(const cv::Mat& original,
                                          const cv::Mat& compressed,
                                          double alpha,
                                          int colormapType) {
    // Generate heatmap
    cv::Mat heatmap = generateHeatmap(original, compressed, colormapType);
    
    // Convert original to color if grayscale
    cv::Mat orig_color;
    if (original.channels() == 1) {
        cv::cvtColor(original, orig_color, cv::COLOR_GRAY2BGR);
    } else {
        orig_color = original.clone();
    }
    
    // Ensure same size
    if (orig_color.size() != heatmap.size()) {
        cv::resize(heatmap, heatmap, orig_color.size());
    }
    
    // Blend
    cv::Mat overlay;
    cv::addWeighted(orig_color, 1.0 - alpha, heatmap, alpha, 0, overlay);
    
    return overlay;
}

HeatmapGenerator::DifferenceStats 
HeatmapGenerator::calculateStats(const cv::Mat& original,
                                const cv::Mat& compressed) {
    // Ensure same size
    cv::Mat comp_resized;
    if (original.size() != compressed.size()) {
        cv::resize(compressed, comp_resized, original.size());
    } else {
        comp_resized = compressed.clone();
    }
    
    // Calculate difference
    cv::Mat diff = calculateDifference(original, comp_resized);
    
    // Calculate statistics
    DifferenceStats stats;
    cv::minMaxLoc(diff, &stats.minError, &stats.maxError);
    
    cv::Scalar meanScalar, stdScalar;
    cv::meanStdDev(diff, meanScalar, stdScalar);
    stats.meanError = meanScalar[0];
    stats.stdError = stdScalar[0];
    
    return stats;
}

void HeatmapGenerator::setThreshold(double threshold) {
    threshold_ = threshold;
}

} // namespace VideoQuality