#include "utils/opencv_ift_bridge.h"
#include <opencv2/opencv.hpp>
#include "ift_result.h"
#include <iostream>

namespace OpenCVIFTBridge {

    Image cvMatToImage(const cv::Mat& mat) {
        // Verifica se é escala de cinza
        if (mat.channels() != 1) {
            throw std::invalid_argument("Mat deve ser escala de cinza (1 canal)");
        }
        
        std::vector<std::vector<uint8_t>> imageData(mat.rows, std::vector<uint8_t>(mat.cols));
        
        for (int i = 0; i < mat.rows; i++) {
            for (int j = 0; j < mat.cols; j++) {
                imageData[i][j] = mat.at<uint8_t>(i, j);
            }
        }
        
        return Image(imageData);
    }

    Image cvMatToImageGray(const cv::Mat& mat) {
        cv::Mat grayMat;
        
        if (mat.channels() == 3) {
            cv::cvtColor(mat, grayMat, cv::COLOR_BGR2GRAY);
        } else if (mat.channels() == 1) {
            grayMat = mat.clone();
        } else {
            throw std::invalid_argument("Mat deve ter 1 ou 3 canais");
        }
        
        return cvMatToImage(grayMat);
    }

    cv::Mat imageToCvMat(const Image& image) {
        cv::Mat mat(image.getHeight(), image.getWidth(), CV_8UC1);
        
        for (int i = 0; i < image.getHeight(); i++) {
            for (int j = 0; j < image.getWidth(); j++) {
                mat.at<uint8_t>(i, j) = image.getPixelValue(j, i);
            }
        }
        
        return mat;
    }

    // VERSÃO SIMPLIFICADA - SEM GUI
    bool selectSeedsInteractive(const cv::Mat& displayImage, SeedSet& seedSet, const Image& image) {
        std::cout << "❌ Modo interativo não disponível nesta versão simplificada." << std::endl;
        std::cout << "Use modo automático: -a <número_de_sementes>" << std::endl;
        return false;
    }

    cv::Mat visualizeIFTResult(const Image& originalImage, const IFTResult& result) {
        cv::Mat visualization(originalImage.getHeight(), originalImage.getWidth(), CV_8UC3);
        
        // Cores para diferentes rótulos
        std::vector<cv::Vec3b> labelColors = {
            cv::Vec3b(0, 0, 0),       // Fundo (preto)
            cv::Vec3b(0, 0, 255),     // Rótulo 1 (vermelho)
            cv::Vec3b(0, 255, 0),     // Rótulo 2 (verde)
            cv::Vec3b(255, 0, 0),     // Rótulo 3 (azul)
            cv::Vec3b(0, 255, 255),   // Rótulo 4 (amarelo)
            cv::Vec3b(255, 0, 255),   // Rótulo 5 (magenta)
            cv::Vec3b(255, 255, 0),   // Rótulo 6 (ciano)
            cv::Vec3b(128, 0, 128),   // Rótulo 7 (roxo)
            cv::Vec3b(255, 165, 0)    // Rótulo 8 (laranja)
        };
        
        for (int y = 0; y < originalImage.getHeight(); y++) {
            for (int x = 0; x < originalImage.getWidth(); x++) {
                Pixel pixel = originalImage.getPixel(x, y);
                int label = result.getLabel(pixel);
                
                if (label >= 0 && label < static_cast<int>(labelColors.size())) {
                    visualization.at<cv::Vec3b>(y, x) = labelColors[label];
                } else {
                    // Cor padrão para rótulos não mapeados
                    uint8_t intensity = originalImage.getPixelValue(x, y);
                    visualization.at<cv::Vec3b>(y, x) = cv::Vec3b(intensity, intensity, intensity);
                }
            }
        }
        
        return visualization;
    }

    bool saveIFTResult(const std::string& filename, const Image& originalImage, const IFTResult& result) {
        cv::Mat visualization = visualizeIFTResult(originalImage, result);
        return cv::imwrite(filename, visualization);
    }

    // VERSÃO SIMPLIFICADA - SEM std::random_device
    SeedSet generateAutomaticSeeds(const Image& image, int numSeeds) {
        SeedSet seedSet;
        
        int width = image.getWidth();
        int height = image.getHeight();
        
        // Distribui sementes nos cantos e centro de forma determinística
        std::vector<std::pair<int, int>> positions;
        
        if (numSeeds >= 1) positions.push_back({width/4, height/4});       // Canto superior esquerdo
        if (numSeeds >= 2) positions.push_back({3*width/4, height/4});     // Canto superior direito
        if (numSeeds >= 3) positions.push_back({width/4, 3*height/4});     // Canto inferior esquerdo
        if (numSeeds >= 4) positions.push_back({3*width/4, 3*height/4});   // Canto inferior direito
        if (numSeeds >= 5) positions.push_back({width/2, height/2});       // Centro
        if (numSeeds >= 6) positions.push_back({width/8, height/2});       // Meio esquerda
        if (numSeeds >= 7) positions.push_back({7*width/8, height/2});     // Meio direita
        if (numSeeds >= 8) positions.push_back({width/2, height/8});       // Meio superior
        
        // Cria sementes
        for (size_t i = 0; i < positions.size() && i < static_cast<size_t>(numSeeds); i++) {
            int x = positions[i].first;
            int y = positions[i].second;
            
            if (image.isValidCoordinate(x, y)) {
                Pixel pixel = image.getPixel(x, y);
                seedSet.addSeed(pixel, static_cast<int>(i + 1), 0);
            }
        }
        
        return seedSet;
    }
} 