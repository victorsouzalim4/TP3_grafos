#ifndef IFT_OPTIMIZED_ALGORITHM_H
#define IFT_OPTIMIZED_ALGORITHM_H

#include <memory>
#include <chrono>
#include <stack>
#include "ift_algorithm.h"
#include "bucket_queue.h"
#include "pixel.h"
#include "image.h"
#include "seed_set.h"
#include "path_cost_function.h"
#include "ift_result.h"

// Algoritmo IFT otimizado (Algoritmo 2) conforme artigo
// Implementa bucket queue para custos inteiros e outras otimizações
class OptimizedIFTAlgorithm : public IFTAlgorithm {
private:
    bool useBucketQueue;      // Se deve usar bucket queue
    bool useIntegerCosts;     // Se custos são inteiros
    int maxCostEstimate;      // Estimativa do custo máximo
    double costDiscretization; // Fator de discretização para custos reais
    
public:
    // Construtor
    OptimizedIFTAlgorithm(bool eightConn = false, bool verbose = false);
    
    // === ALGORITMO 2 (OTIMIZADO) ===
    
    // Executa Algoritmo 2 com bucket queue para custos inteiros
    // Complexidade: O(m + nK) onde K é o range de custos
    std::unique_ptr<IFTResult> runOptimizedIFT(
        const Image& image,
        const PathCostFunction& costFunction,
        const SeedSet& seeds
    );
    
    // Versão com discretização para custos reais
    std::unique_ptr<IFTResult> runDiscretizedIFT(
        const Image& image,
        const PathCostFunction& costFunction,
        const SeedSet& seeds,
        double precision = 0.1
    );
    
    // Versão híbrida (bucket + heap)
    std::unique_ptr<IFTResult> runHybridIFT(
        const Image& image,
        const PathCostFunction& costFunction,
        const SeedSet& seeds
    );
    
    // === CONFIGURAÇÕES OTIMIZADAS ===
    
    void setUseBucketQueue(bool use) { useBucketQueue = use; }
    bool getUseBucketQueue() const { return useBucketQueue; }
    
    void setMaxCostEstimate(int maxCost) { maxCostEstimate = maxCost; }
    int getMaxCostEstimate() const { return maxCostEstimate; }
    
    void setCostDiscretization(double disc) { costDiscretization = disc; }
    double getCostDiscretization() const { return costDiscretization; }
    
    // === ANÁLISE DE PERFORMANCE ===
    
    // Estatísticas estendidas para algoritmo otimizado
    struct OptimizedStats : public ExecutionStats {
        bool usedBucketQueue;
        bool usedDiscretization;
        double bucketQueueUtilization;  // % do tempo usando bucket queue
        int maxCostObserved;
        size_t bucketOperations;
        size_t heapOperations;
        double memoryUsageMB;
        
        void print() const;
    };
    
    OptimizedStats getLastOptimizedStats() const { return lastOptStats; }
    
    // Benchmark automático para escolher melhor implementação
    struct AlgorithmBenchmark {
        double basicIFTTimeMs;
        double optimizedIFTTimeMs;
        double hybridIFTTimeMs;
        size_t imageSize;
        std::string bestAlgorithm;
        double speedupFactor;
        
        void print() const;
    };
    
    AlgorithmBenchmark benchmarkAlgorithms(
        const Image& image,
        const PathCostFunction& costFunction,
        const SeedSet& seeds
    );
    
    // Estima custo máximo baseado na função e imagem
    int estimateMaxCost(const PathCostFunction& costFunc, const Image& image);
    
private:
    OptimizedStats lastOptStats;
    
    // === MÉTODOS INTERNOS OTIMIZADOS ===
    
    // Detecta automaticamente se custos são inteiros
    bool analyzeCostFunction(const PathCostFunction& costFunc, const Image& image);
    
    // Loop principal otimizado com bucket queue
    void processOptimizedMainLoop(
        IFTResult& result,
        const Image& image,
        const PathCostFunction& costFunction,
        BucketQueue& bucketQueue
    );
    
    // Loop principal híbrido
    void processHybridMainLoop(
        IFTResult& result,
        const Image& image,
        const PathCostFunction& costFunction,
        HybridPriorityQueue& hybridQueue
    );
    
    // Processa vizinhos com bucket queue
    void processNeighborsOptimized(
        const Pixel& currentPixel,
        IFTResult& result,
        const Image& image,
        const PathCostFunction& costFunction,
        BucketQueue& bucketQueue
    );
    
    // Atualiza custo com versão otimizada
    bool updatePixelCostOptimized(
        const Pixel& fromPixel,
        const Pixel& toPixel,
        IFTResult& result,
        const PathCostFunction& costFunction,
        const Image& image,
        BucketQueue& bucketQueue
    );
};

// === ALGORITMO 3 (LIFO TIE-BREAKING) ===

// Algoritmo IFT com política LIFO para tie-breaking
// Útil para aplicações específicas como boundary tracking
class LIFOIFTAlgorithm : public IFTAlgorithm {
private:
    // Stack para tie-breaking LIFO
    std::stack<Pixel> tieBreakingStack;
    
public:
    LIFOIFTAlgorithm(bool eightConn = false, bool verbose = false);
    
    // Executa Algoritmo 3 com tie-breaking LIFO
    std::unique_ptr<IFTResult> runLIFOIFT(
        const Image& image,
        const PathCostFunction& costFunction,
        const SeedSet& seeds
    );
    
    // === CONFIGURAÇÕES LIFO ===
    
    enum class TieBreakingPolicy {
        FIFO,     // First-In-First-Out (padrão)
        LIFO,     // Last-In-First-Out
        RANDOM    // Aleatório
    };
    
    void setTieBreakingPolicy(TieBreakingPolicy policy);
    TieBreakingPolicy getTieBreakingPolicy() const;
    
private:
    TieBreakingPolicy tiePolicy;
    
    // Loop principal com tie-breaking customizado
    void processLIFOMainLoop(
        IFTResult& result,
        const Image& image,
        const PathCostFunction& costFunction
    );
    
    // Estrutura para tie-breaking avançado
    struct TieBreakingQueue {
        std::vector<std::vector<Pixel>> costBuckets;  // Buckets por custo
        int currentCost;
        TieBreakingPolicy policy;
        
        void push(const Pixel& pixel, int cost);
        Pixel pop();
        bool empty() const;
        void clear();
    };
    
    TieBreakingQueue tieQueue;
};

// === FACTORY E UTILITÁRIOS ===

// Factory para criar algoritmo otimizado automaticamente
std::unique_ptr<OptimizedIFTAlgorithm> createAutoOptimizedIFT(
    const Image& image,
    const PathCostFunction& costFunction,
    bool eightConnected = false
);

// Comparação completa de todos os algoritmos
struct IFTAlgorithmComparison {
    struct AlgorithmResult {
        std::string name;
        double executionTimeMs;
        std::unique_ptr<IFTResult> result;
        bool isCorrect;
        double memoryUsageMB;
    };
    
    std::vector<AlgorithmResult> results;
    std::string bestAlgorithm;
    double bestTimeMs;
    
    void addResult(const std::string& name, double time, 
                   std::unique_ptr<IFTResult> result, bool correct = true);
    void runComparison(const Image& image, const PathCostFunction& costFunc, 
                      const SeedSet& seeds);
    void print() const;
};

// Teste de estresse para validar otimizações
struct StressTestResult {
    size_t imageSize;
    size_t seedCount;
    double basicTimeMs;
    double optimizedTimeMs;
    double speedup;
    bool resultsMatch;
    
    void print() const;
};

StressTestResult runStressTest(int imageSize, int seedCount, 
                              const std::string& costFunctionType = "intensity_diff");

#endif 