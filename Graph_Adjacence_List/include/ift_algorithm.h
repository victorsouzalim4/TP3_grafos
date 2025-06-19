#ifndef IFT_ALGORITHM_H
#define IFT_ALGORITHM_H

#include <queue>
#include <memory>
#include "pixel.h"
#include "image.h"
#include "seed_set.h"
#include "path_cost_function.h"
#include "ift_result.h"

// Comparador para priority queue (fila de prioridade por custo)
struct PixelCostComparator {
    const std::unordered_map<Pixel, double, PixelHash>* costMap;
    
    PixelCostComparator(const std::unordered_map<Pixel, double, PixelHash>* costs) 
        : costMap(costs) {}
    
    bool operator()(const Pixel& a, const Pixel& b) const {
        auto itA = costMap->find(a);
        auto itB = costMap->find(b);
        
        double costA = (itA != costMap->end()) ? itA->second : std::numeric_limits<double>::infinity();
        double costB = (itB != costMap->end()) ? itB->second : std::numeric_limits<double>::infinity();
        
        return costA > costB; // Min-heap: menor custo tem prioridade
    }
};

// Implementação do algoritmo IFT básico
class IFTAlgorithm {
protected:
    bool eightConnected;  // Conectividade: 4 ou 8
    bool verbose;         // Debug output
    
public:
    // Construtor
    IFTAlgorithm(bool eightConn = false, bool verb = false) 
        : eightConnected(eightConn), verbose(verb) {}
    
    // === ALGORITMO PRINCIPAL ===
    
    // Executa Algoritmo 1 (básico) conforme artigo IFT
    // ENTRADA: Grafo G=(I,A), função f, conjunto S ⊆ I  
    // SAÍDA: Floresta de caminhos ótimos (P, C, L)
    std::unique_ptr<IFTResult> runBasicIFT(
        const Image& image,
        const PathCostFunction& costFunction,
        const SeedSet& seeds
    );
    
    // Alias para compatibilidade
    std::unique_ptr<IFTResult> runIFT(
        const Image& image,
        const PathCostFunction& costFunction,
        const SeedSet& seeds
    ) {
        return runBasicIFT(image, costFunction, seeds);
    }
    
    // === VARIAÇÕES DO ALGORITMO ===
    
    // Versão com early termination quando target é alcançado
    std::unique_ptr<IFTResult> runIFTToTarget(
        const Image& image,
        const PathCostFunction& costFunction,
        const SeedSet& seeds,
        const Pixel& target
    );
    
    // Versão que processa apenas região de interesse (ROI)
    std::unique_ptr<IFTResult> runIFTInRegion(
        const Image& image,
        const PathCostFunction& costFunction,
        const SeedSet& seeds,
        int startX, int startY, int width, int height
    );
    
    // === CONFIGURAÇÕES ===
    
    void setConnectivity(bool eightConn) { eightConnected = eightConn; }
    bool getConnectivity() const { return eightConnected; }
    
    void setVerbose(bool verb) { verbose = verb; }
    bool getVerbose() const { return verbose; }
    
    // === ANÁLISE E DEBUG ===
    
    // Valida resultado contra propriedades teóricas
    bool validateResult(
        const IFTResult& result,
        const Image& image,
        const PathCostFunction& costFunction,
        const SeedSet& seeds
    ) const;
    
    // Estatísticas de execução
    struct ExecutionStats {
        size_t pixelsProcessed;
        size_t iterationsTotal;
        double executionTimeMs;
        double averageCostPerPixel;
        bool isComplete;
        bool isValid;
        
        void print() const;
    };
    
    ExecutionStats getLastExecutionStats() const { return lastStats; }
    
protected:
    ExecutionStats lastStats;  // Estatísticas da última execução
    
    // === MÉTODOS AUXILIARES INTERNOS ===
    
    // Inicializa mapeamentos IFT (P, C, L)
    void initializeIFTMaps(
        IFTResult& result,
        const Image& image,
        const PathCostFunction& costFunction,
        const SeedSet& seeds
    );
    
    // Implementação do loop principal do Algoritmo 1
    void processIFTMainLoop(
        IFTResult& result,
        const Image& image,
        const PathCostFunction& costFunction,
        std::priority_queue<Pixel, std::vector<Pixel>, PixelCostComparator>& queue
    );
    
    // Processa vizinhos de um pixel
    void processNeighbors(
        const Pixel& currentPixel,
        IFTResult& result,
        const Image& image,
        const PathCostFunction& costFunction,
        std::priority_queue<Pixel, std::vector<Pixel>, PixelCostComparator>& queue
    );
    
    // Versão simplificada do processNeighbors para algoritmos otimizados
    template<typename QueueType>
    void processNeighbors(
        const Pixel& currentPixel,
        IFTResult& result,
        const Image& image,
        const PathCostFunction& costFunction,
        QueueType queuePushFunction
    );
    
    // Atualiza custo de um pixel vizinho
    bool updatePixelCost(
        const Pixel& fromPixel,
        const Pixel& toPixel,
        IFTResult& result,
        const PathCostFunction& costFunction,
        const Image& image
    );
    
    // Debug: imprime estado atual do algoritmo
    void printAlgorithmState(
        const IFTResult& result,
        const Pixel& currentPixel,
        int iteration
    ) const;
};

// === FUNÇÕES UTILITÁRIAS ===

// Factory para criar algoritmo com configurações comuns
std::unique_ptr<IFTAlgorithm> createStandardIFT(bool eightConnected = false);
std::unique_ptr<IFTAlgorithm> createVerboseIFT(bool eightConnected = false);

// Execução rápida para casos simples
std::unique_ptr<IFTResult> quickIFT(
    const Image& image,
    const PathCostFunction& costFunction,
    const SeedSet& seeds,
    bool eightConnected = false
);

// Comparação de diferentes algoritmos/configurações
struct IFTComparison {
    std::vector<std::unique_ptr<IFTResult>> results;
    std::vector<IFTAlgorithm::ExecutionStats> stats;
    std::vector<std::string> configNames;
    
    void addResult(std::unique_ptr<IFTResult> result, 
                   const IFTAlgorithm::ExecutionStats& stat,
                   const std::string& name);
    
    void printComparison() const;
    std::unique_ptr<IFTResult> getBestResult() const; // Por tempo de execução
};

#endif 