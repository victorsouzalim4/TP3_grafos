#ifndef OPENCV_IFT_BRIDGE_H
#define OPENCV_IFT_BRIDGE_H

#include <opencv2/opencv.hpp>
#include <string>
#include "image.h"
#include "seed_set.h"
#include "ift_result.h"

namespace OpenCVIFTBridge {
    
    /**
     * Converte cv::Mat (escala de cinza) para Image
     */
    Image cvMatToImage(const cv::Mat& mat);
    
    /**
     * Converte cv::Mat (qualquer formato) para Image em escala de cinza
     */
    Image cvMatToImageGray(const cv::Mat& mat);
    
    /**
     * Converte Image para cv::Mat
     */
    cv::Mat imageToCvMat(const Image& image);
    
    /**
     * Seleção interativa de sementes (versão simplificada retorna false)
     */
    bool selectSeedsInteractive(const cv::Mat& displayImage, SeedSet& seedSet, const Image& image);
    
    /**
     * Visualiza resultado do IFT como imagem colorida
     */
    cv::Mat visualizeIFTResult(const Image& originalImage, const IFTResult& result);
    
    /**
     * Salva resultado do IFT como arquivo de imagem
     */
    bool saveIFTResult(const std::string& filename, const Image& originalImage, const IFTResult& result);
    
    /**
     * Gera sementes automáticas distribuídas pela imagem
     */
    SeedSet generateAutomaticSeeds(const Image& image, int numSeeds);
}

#endif // OPENCV_IFT_BRIDGE_H 