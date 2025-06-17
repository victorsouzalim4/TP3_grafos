#ifndef IMAGE_TO_GRAPH_H
#define IMAGE_TO_GRAPH_H

#include "undirected_Graph.h"
#include <vector>
#include <array>
#include <string>
#include <cstdint>

class ImageGraphConverter {
public:
    // Para imagem colorida RGB
    static void imageToGraphRGB(const std::vector<std::vector<std::array<uint8_t, 3>>>& image, UndirectedGraph& graph, bool eightConnected = false);

    // Para imagem em tons de cinza
    static void imageToGraphGray(const std::vector<std::vector<uint8_t>>& image, UndirectedGraph& graph, bool eightConnected = false);
};

#endif
