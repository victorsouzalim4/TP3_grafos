#include "Utils/imageToGraph.h"
#include <cmath>

static double rgbDistance(const std::array<uint8_t, 3>& a, const std::array<uint8_t, 3>& b) {
    return std::sqrt(
        (a[0] - b[0]) * (a[0] - b[0]) +
        (a[1] - b[1]) * (a[1] - b[1]) +
        (a[2] - b[2]) * (a[2] - b[2])
    );
}

static double grayDistance(uint8_t a, uint8_t b) {
    return std::abs(a - b);
}

void ImageGraphConverter::imageToGraphRGB(
    const std::vector<std::vector<std::array<uint8_t, 3>>>& image,
    UndirectedGraph& graph,
    bool eightConnected
) {
    int rows = image.size();
    int cols = image[0].size();

    std::vector<std::pair<int, int>> neighbors;
    neighbors.push_back(std::make_pair(0, 1));
    neighbors.push_back(std::make_pair(1, 0));
    if (eightConnected) {
        neighbors.push_back(std::make_pair(1, 1));
        neighbors.push_back(std::make_pair(-1, 1));
    }

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            int u = y * cols + x;
            std::string labelU = std::to_string(u);
            graph.addVertex(labelU);

            for (size_t i = 0; i < neighbors.size(); ++i) {
                int dy = neighbors[i].first;
                int dx = neighbors[i].second;

                int ny = y + dy;
                int nx = x + dx;
                if (ny >= 0 && ny < rows && nx >= 0 && nx < cols) {
                    int v = ny * cols + nx;
                    std::string labelV = std::to_string(v);

                    double weight = rgbDistance(image[y][x], image[ny][nx]);
                    graph.addVertex(labelV);
                    graph.addEdge(labelU, labelV, weight);
                }
            }
        }
    }
}

void ImageGraphConverter::imageToGraphGray(
    const std::vector<std::vector<uint8_t>>& image,
    UndirectedGraph& graph,
    bool eightConnected
) {
    int rows = image.size();
    int cols = image[0].size();

    std::vector<std::pair<int, int>> neighbors;
    neighbors.push_back(std::make_pair(0, 1));
    neighbors.push_back(std::make_pair(1, 0));
    if (eightConnected) {
        neighbors.push_back(std::make_pair(1, 1));
        neighbors.push_back(std::make_pair(-1, 1));
    }

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            int u = y * cols + x;
            std::string labelU = std::to_string(u);
            graph.addVertex(labelU);

            for (size_t i = 0; i < neighbors.size(); ++i) {
                int dy = neighbors[i].first;
                int dx = neighbors[i].second;

                int ny = y + dy;
                int nx = x + dx;
                if (ny >= 0 && ny < rows && nx >= 0 && nx < cols) {
                    int v = ny * cols + nx;
                    std::string labelV = std::to_string(v);

                    double weight = grayDistance(image[y][x], image[ny][nx]);
                    graph.addVertex(labelV);
                    graph.addEdge(labelU, labelV, weight);
                }
            }
        }
    }
}
