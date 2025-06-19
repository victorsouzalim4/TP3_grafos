#ifndef COLORSEGMENTATION_H
#define COLORSEGMENTATION_H

#include <opencv2/opencv.hpp>
#include <unordered_map>
#include <vector>
#include <random>

class ColorSegmentation {
public:
    static cv::Mat colorSegments(const cv::Mat& image, const std::vector<int>& componentIds);
};

#endif
