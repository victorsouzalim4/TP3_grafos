#include "Utils/colorSegments.h"
#include <opencv2/opencv.hpp>
#include <unordered_map>
#include <vector>
#include <random>

// Gera imagem com segmentos coloridos
cv::Mat ColorSegmentation::colorSegments(const cv::Mat& image, const std::vector<int>& componentIds) {
    int rows = image.rows;
    int cols = image.cols;
    CV_Assert(rows * cols == componentIds.size());

    cv::Mat output(rows, cols, CV_8UC3);  // imagem de saída colorida

    // Gerar cores distintas para cada componente
    std::unordered_map<int, cv::Vec3b> colorMap;
    std::mt19937 rng(123);  // semente para gerar cores
    std::uniform_int_distribution<int> dist(0, 255);

    for (int id : componentIds) {
        if (colorMap.find(id) == colorMap.end()) {
            colorMap[id] = cv::Vec3b(dist(rng), dist(rng), dist(rng));
        }
    }

    // Preencher imagem de saída com cores de componentes
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int idx = i * cols + j;
            int compId = componentIds[idx];
            output.at<cv::Vec3b>(i, j) = colorMap[compId];
        }
    }

    return output;
}
