#include <iostream>
#include "directed_Graph.h"
#include "Undirected_Graph.h"
#include "Utils/segmentation.h"
#include "utils/imageToGraph.h"
#include "vertex.h"
#include "utils/colorSegments.h"

#include <opencv2/opencv.hpp> // Novo include

using namespace std;

int main()
{

    UndirectedGraph g;
    Segmentation s;
    ColorSegmentation c;

    // Carrega imagem
    cv::Mat img = cv::imread("./src/images/imagem2.png", cv::IMREAD_COLOR);
    if (img.empty())
    {
        cerr << "Erro ao carregar a imagem!" << endl;
        return -1;
    }

    cv::GaussianBlur(img, img, cv::Size(0, 0), 0.8); // sigma = 0.8

    int linhas = img.rows;
    int colunas = img.cols;

    // Converte cv::Mat para std::vector<std::vector<std::array<uint8_t, 3>>>
    std::vector<std::vector<std::array<uint8_t, 3>>> image(linhas, std::vector<std::array<uint8_t, 3>>(colunas));

    for (int i = 0; i < linhas; ++i)
    {
        for (int j = 0; j < colunas; ++j)
        {
            cv::Vec3b pixel = img.at<cv::Vec3b>(i, j);
            // OpenCV usa BGR, reordenamos para RGB
            image[i][j] = {pixel[2], pixel[1], pixel[0]};
        }
    }

    ImageGraphConverter::imageToGraphRGB(image, g, false);

    g.print(); // Ver conexões com pesos entre cores

    std::vector<int> componentIds = s.segmentGraph(g, 1000, 100);

    cv::Mat imgProcess = c.colorSegments(img, componentIds);

    // Exibir resultado
    cv::imshow("Segmentos", imgProcess);
    cv::waitKey(0);

    return 0;
}