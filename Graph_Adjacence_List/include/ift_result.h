#ifndef IFT_RESULT_H
#define IFT_RESULT_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <limits>
#include "pixel.h"
#include "image.h"
#include "seed_set.h"

// Representa o resultado de uma execução do algoritmo IFT
// Conforme artigo: "forest of optimum paths (P, C, L)"
class IFTResult {
private:
    int width, height;                                              // Dimensões da imagem
    std::unordered_map<Pixel, Pixel, PixelHash> predecessorMap;    // P(t) - predecessor de cada pixel
    std::unordered_map<Pixel, double, PixelHash> costMap;          // C(t) - custo ótimo para cada pixel  
    std::unordered_map<Pixel, int, PixelHash> labelMap;            // L(t) - label da raiz para cada pixel
    std::vector<Pixel> seedPixels;                                 // Pixels que foram sementes S ⊆ I
    
public:
    // Construtor
    IFTResult(int w, int h) : width(w), height(h) {}
    
    // === ACESSO AOS MAPS FUNDAMENTAIS (P, C, L) ===
    
    // P(t) - Predecessor: retorna predecessor de t na floresta ótima
    Pixel getPredecessor(const Pixel& pixel) const;
    void setPredecessor(const Pixel& pixel, const Pixel& predecessor);
    bool hasPredecessor(const Pixel& pixel) const;
    
    // C(t) - Custo: retorna custo ótimo do caminho até t
    double getCost(const Pixel& pixel) const;
    void setCost(const Pixel& pixel, double cost);
    
    // L(t) - Label: retorna label da raiz do caminho até t
    int getLabel(const Pixel& pixel) const;
    void setLabel(const Pixel& pixel, int label);
    bool hasLabel(const Pixel& pixel) const;
    
    // === CONSULTAS DE CAMINHO ===
    
    // Reconstrói caminho ótimo da raiz até pixel
    std::vector<Pixel> getOptimalPath(const Pixel& pixel) const;
    
    // Obtém pixel raiz do caminho para determinado pixel
    Pixel getRootPixel(const Pixel& pixel) const;
    
    // Verifica se pixel é uma raiz (sem predecessor)
    bool isRoot(const Pixel& pixel) const;
    
    // === SEGMENTAÇÃO E ROTULAÇÃO ===
    
    // Cria imagem de segmentação com labels
    Image createSegmentationImage() const;
    
    // Cria imagem de custos (para visualização)
    Image createCostImage() const;
    
    // Obtém todos os pixels com determinado label
    std::vector<Pixel> getPixelsWithLabel(int label) const;
    
    // Obtém todos os labels únicos presentes
    std::vector<int> getUniqueLabels() const;
    
    // === ESTATÍSTICAS E ANÁLISE ===
    
    // Número de pixels processados (com custo finito)
    size_t getProcessedPixelCount() const;
    size_t getPixelsProcessed() const { return getProcessedPixelCount(); } // Alias
    void incrementPixelsProcessed() { /* implementado no .cpp */ }
    
    // Custo mínimo, máximo e médio
    double getMinCost() const;
    double getMaxCost() const;
    double getAverageCost() const;
    
    // Número de componentes conectados (árvores na floresta)
    int getComponentCount() const;
    
    // === VALIDAÇÃO ===
    
    // Verifica se resultado é uma floresta válida
    bool isValidForest() const;
    
    // Verifica se todos os pixels têm custo finito
    bool isComplete() const;
    
    // === DEBUG E VISUALIZAÇÃO ===
    
    void print() const;
    void printStatistics() const;
    void saveToFile(const std::string& filename) const;
    
    // Salva mapa de custos como imagem PGM
    bool saveCostMap(const std::string& filename) const;
    
    // Salva segmentação como imagem PPM colorida
    bool saveSegmentation(const std::string& filename) const;
    
    // === ACESSO INTERNO (para algoritmos) ===
    
    // Acesso direto aos maps para eficiência durante processamento
    std::unordered_map<Pixel, Pixel, PixelHash>& getPredecessorMapRef() { return predecessorMap; }
    std::unordered_map<Pixel, double, PixelHash>& getCostMapRef() { return costMap; }
    std::unordered_map<Pixel, int, PixelHash>& getLabelMapRef() { return labelMap; }
    
    // Inicialização para algoritmo
    void initializeForProcessing(const Image& image, const SeedSet& seeds);
    
    // Marca pixel como semente processada
    void addSeedPixel(const Pixel& pixel) { seedPixels.push_back(pixel); }
    
    // Dimensões
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

// === FUNÇÕES AUXILIARES ===

// Compara resultados IFT (para testes)  
bool compareIFTResults(const IFTResult& result1, const IFTResult& result2, double tolerance = 1e-6);

// Valida resultado contra propriedades teóricas do IFT
bool validateIFTProperties(const IFTResult& result, const Image& image, 
                          const class PathCostFunction& costFunc, const SeedSet& seeds);

// Cria visualização em ASCII da floresta (para debug)
std::string visualizeForest(const IFTResult& result, const Image& image);

#endif 