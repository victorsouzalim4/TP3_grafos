#include "ift_result.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <climits>
#include <sstream>

// === IMPLEMENTAÇÃO DOS MÉTODOS FUNDAMENTAIS (P, C, L) ===

// P(t) - Predecessor
Pixel IFTResult::getPredecessor(const Pixel& pixel) const {
    auto it = predecessorMap.find(pixel);
    if (it != predecessorMap.end()) {
        return it->second;
    }
    return Pixel(); // Pixel inválido se não tem predecessor
}

void IFTResult::setPredecessor(const Pixel& pixel, const Pixel& predecessor) {
    predecessorMap[pixel] = predecessor;
}

bool IFTResult::hasPredecessor(const Pixel& pixel) const {
    return predecessorMap.find(pixel) != predecessorMap.end();
}

// C(t) - Custo
double IFTResult::getCost(const Pixel& pixel) const {
    auto it = costMap.find(pixel);
    if (it != costMap.end()) {
        return it->second;
    }
    return std::numeric_limits<double>::infinity(); // +∞ se não processado
}

void IFTResult::setCost(const Pixel& pixel, double cost) {
    costMap[pixel] = cost;
}

// L(t) - Label  
int IFTResult::getLabel(const Pixel& pixel) const {
    auto it = labelMap.find(pixel);
    if (it != labelMap.end()) {
        return it->second;
    }
    return -1; // Label inválido se não tem
}

void IFTResult::setLabel(const Pixel& pixel, int label) {
    labelMap[pixel] = label;
}

bool IFTResult::hasLabel(const Pixel& pixel) const {
    return labelMap.find(pixel) != labelMap.end();
}

// === CONSULTAS DE CAMINHO ===

std::vector<Pixel> IFTResult::getOptimalPath(const Pixel& pixel) const {
    std::vector<Pixel> path;
    Pixel current = pixel;
    
    // Reconstrói caminho seguindo predecessores
    while (hasPredecessor(current)) {
        path.push_back(current);
        current = getPredecessor(current);
    }
    
    // Adiciona a raiz
    if (costMap.find(current) != costMap.end()) {
        path.push_back(current);
    }
    
    // Inverte para ter caminho da raiz ao pixel
    std::reverse(path.begin(), path.end());
    return path;
}

Pixel IFTResult::getRootPixel(const Pixel& pixel) const {
    Pixel current = pixel;
    
    // Segue predecessores até a raiz
    while (hasPredecessor(current)) {
        current = getPredecessor(current);
    }
    
    return current;
}

bool IFTResult::isRoot(const Pixel& pixel) const {
    return !hasPredecessor(pixel) && (costMap.find(pixel) != costMap.end());
}

// === SEGMENTAÇÃO E ROTULAÇÃO ===

Image IFTResult::createSegmentationImage() const {
    Image segImg(width, height, 0);
    
    for (const auto& pair : labelMap) {
        const Pixel& pixel = pair.first;
        int label = pair.second;
        
        // Usa label como intensidade (limitado a 255)
        uint8_t intensity = static_cast<uint8_t>(std::min(label, 255));
        segImg.setPixelValue(pixel.x, pixel.y, intensity);
    }
    
    return segImg;
}

Image IFTResult::createCostImage() const {
    Image costImg(width, height, 0);
    
    // Encontra custo máximo para normalização
    double maxCost = getMaxCost();
    if (maxCost == std::numeric_limits<double>::infinity() || maxCost == 0) {
        return costImg; // Retorna imagem zerada se custos são inválidos
    }
    
    for (const auto& pair : costMap) {
        const Pixel& pixel = pair.first;
        double cost = pair.second;
        
        if (cost != std::numeric_limits<double>::infinity()) {
            // Normaliza custo para [0, 255]
            uint8_t intensity = static_cast<uint8_t>((cost / maxCost) * 255);
            costImg.setPixelValue(pixel.x, pixel.y, intensity);
        }
    }
    
    return costImg;
}

std::vector<Pixel> IFTResult::getPixelsWithLabel(int label) const {
    std::vector<Pixel> pixels;
    
    for (const auto& pair : labelMap) {
        if (pair.second == label) {
            pixels.push_back(pair.first);
        }
    }
    
    return pixels;
}

std::vector<int> IFTResult::getUniqueLabels() const {
    std::vector<int> labels;
    
    for (const auto& pair : labelMap) {
        int label = pair.second;
        if (std::find(labels.begin(), labels.end(), label) == labels.end()) {
            labels.push_back(label);
        }
    }
    
    std::sort(labels.begin(), labels.end());
    return labels;
}

// === ESTATÍSTICAS E ANÁLISE ===

size_t IFTResult::getProcessedPixelCount() const {
    size_t count = 0;
    for (const auto& pair : costMap) {
        if (pair.second != std::numeric_limits<double>::infinity()) {
            count++;
        }
    }
    return count;
}

double IFTResult::getMinCost() const {
    double minCost = std::numeric_limits<double>::infinity();
    
    for (const auto& pair : costMap) {
        if (pair.second < minCost && pair.second != std::numeric_limits<double>::infinity()) {
            minCost = pair.second;
        }
    }
    
    return minCost;
}

double IFTResult::getMaxCost() const {
    double maxCost = 0.0;
    
    for (const auto& pair : costMap) {
        if (pair.second > maxCost && pair.second != std::numeric_limits<double>::infinity()) {
            maxCost = pair.second;
        }
    }
    
    return maxCost;
}

double IFTResult::getAverageCost() const {
    double sum = 0.0;
    size_t count = 0;
    
    for (const auto& pair : costMap) {
        if (pair.second != std::numeric_limits<double>::infinity()) {
            sum += pair.second;
            count++;
        }
    }
    
    return count > 0 ? sum / count : 0.0;
}

int IFTResult::getComponentCount() const {
    return static_cast<int>(seedPixels.size());
}

// === VALIDAÇÃO ===

bool IFTResult::isValidForest() const {
    // Verifica se não há ciclos
    for (const auto& pair : predecessorMap) {
        const Pixel& pixel = pair.first;
        
        // Tenta seguir caminho até raiz
        Pixel current = pixel;
        std::vector<Pixel> visited;
        
        while (hasPredecessor(current)) {
            if (std::find(visited.begin(), visited.end(), current) != visited.end()) {
                return false; // Ciclo detectado
            }
            visited.push_back(current);
            current = getPredecessor(current);
        }
    }
    
    return true;
}

bool IFTResult::isComplete() const {
    for (const auto& pair : costMap) {
        if (pair.second == std::numeric_limits<double>::infinity()) {
            return false;
        }
    }
    return !costMap.empty();
}

// === INICIALIZAÇÃO ===

void IFTResult::initializeForProcessing(const Image& image, const SeedSet& seeds) {
    // Limpa estruturas
    predecessorMap.clear();
    costMap.clear();
    labelMap.clear();
    seedPixels.clear();
    
    // Inicializa todos os pixels com custo infinito
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Pixel pixel = image.getPixel(x, y);
            costMap[pixel] = std::numeric_limits<double>::infinity();
        }
    }
    
    // Inicializa sementes conforme artigo IFT
    auto activeSeeds = seeds.getActiveSeeds();
    for (const auto& seed : activeSeeds) {
        const Pixel& seedPixel = seed.pixel;
        
        // C(s) ← h(s), L(s) ← s (conforme Algoritmo 1)
        costMap[seedPixel] = seed.handicap;
        labelMap[seedPixel] = seed.label;
        seedPixels.push_back(seedPixel);
    }
}

// === DEBUG E VISUALIZAÇÃO ===

void IFTResult::print() const {
    std::cout << "=== IFT RESULT ===" << std::endl;
    std::cout << "Dimensions: " << width << "x" << height << std::endl;
    std::cout << "Processed pixels: " << getProcessedPixelCount() << std::endl;
    std::cout << "Components: " << getComponentCount() << std::endl;
    
    if (!costMap.empty()) {
        std::cout << "Cost range: [" << getMinCost() << ", " << getMaxCost() << "]" << std::endl;
        std::cout << "Average cost: " << getAverageCost() << std::endl;
    }
    
    std::cout << "Unique labels: ";
    auto labels = getUniqueLabels();
    for (size_t i = 0; i < labels.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << labels[i];
    }
    std::cout << std::endl;
    
    std::cout << "Is valid forest: " << (isValidForest() ? "Yes" : "No") << std::endl;
    std::cout << "Is complete: " << (isComplete() ? "Yes" : "No") << std::endl;
    std::cout << "==================" << std::endl;
}

void IFTResult::printStatistics() const {
    std::cout << "\n=== IFT STATISTICS ===" << std::endl;
    
    auto labels = getUniqueLabels();
    for (int label : labels) {
        auto pixels = getPixelsWithLabel(label);
        std::cout << "Label " << label << ": " << pixels.size() << " pixels" << std::endl;
    }
    
    std::cout << "\nSeeds processed: ";
    for (size_t i = 0; i < seedPixels.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << seedPixels[i].toString();
    }
    std::cout << std::endl;
    std::cout << "======================" << std::endl;
}

// === FUNÇÕES AUXILIARES ===

bool compareIFTResults(const IFTResult& result1, const IFTResult& result2, double tolerance) {
    if (result1.getWidth() != result2.getWidth() || result1.getHeight() != result2.getHeight()) {
        return false;
    }
    
    // Compara custos
    for (int y = 0; y < result1.getHeight(); ++y) {
        for (int x = 0; x < result1.getWidth(); ++x) {
            Pixel pixel(x, y, 0); // Intensidade não importa para comparação
            
            double cost1 = result1.getCost(pixel);
            double cost2 = result2.getCost(pixel);
            
            if (std::abs(cost1 - cost2) > tolerance) {
                return false;
            }
        }
    }
    
    return true;
}

std::string visualizeForest(const IFTResult& result, const Image& image) {
    std::ostringstream oss;
    
    oss << "IFT Forest Visualization:\n";
    oss << std::string(result.getWidth() * 4, '-') << "\n";
    
    for (int y = 0; y < result.getHeight(); ++y) {
        for (int x = 0; x < result.getWidth(); ++x) {
            Pixel pixel = image.getPixel(x, y);
            
            if (result.isRoot(pixel)) {
                oss << " R  ";  // Root
            } else if (result.hasPredecessor(pixel)) {
                oss << " *  ";  // Processado
            } else {
                oss << " .  ";  // Não processado
            }
        }
        oss << "\n";
    }
    
    return oss.str();
} 