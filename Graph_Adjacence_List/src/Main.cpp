#include <iostream>
#include "directed_Graph.h"
#include "Undirected_Graph.h"
#include "Utils/segmentation.h"
#include "utils/imageToGraph.h"
#include "vertex.h"

#include <opencv2/opencv.hpp> // Novo include

using namespace std;

int main()
{

    UndirectedGraph g;
    Segmentation s;

    // Carrega imagem
    cv::Mat img = cv::imread("./src/images/imagem3.jpg", cv::IMREAD_COLOR);
    if (img.empty())
    {
        cerr << "Erro ao carregar a imagem!" << endl;
        return -1;
    }

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

    s.segmentGraph(g, 300);



    return 0;
}