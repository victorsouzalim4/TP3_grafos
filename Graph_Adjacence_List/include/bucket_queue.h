#ifndef BUCKET_QUEUE_H
#define BUCKET_QUEUE_H

#include <vector>
#include <queue>
#include <limits>
#include <iostream>
#include <memory>
#include "pixel.h"

// Bucket Queue otimizada para custos inteiros conforme artigo IFT
// Complexidade: O(m + nK) onde K é o range de custos
class BucketQueue {
private:
    std::vector<std::queue<Pixel>> buckets;  // Array de filas por custo
    int minBucket;                           // Bucket com menor custo atual
    int maxBucket;                           // Bucket com maior custo válido
    int bucketCount;                         // Número total de buckets
    size_t totalElements;                    // Total de elementos na fila
    
    // Encontra próximo bucket não-vazio a partir de minBucket
    void updateMinBucket();
    
public:
    // Construtor: range de custos [0, maxCost]
    BucketQueue(int maxCost);
    
    // === OPERAÇÕES BÁSICAS ===
    
    // Adiciona pixel com custo específico
    void push(const Pixel& pixel, int cost);
    
    // Remove e retorna pixel com menor custo
    Pixel pop();
    
    // Retorna pixel com menor custo sem remover
    const Pixel& top() const;
    
    // Verifica se fila está vazia
    bool empty() const { return totalElements == 0; }
    
    // Número de elementos
    size_t size() const { return totalElements; }
    
    // === OTIMIZAÇÕES ESPECÍFICAS IFT ===
    
    // Retorna custo mínimo atual
    int getMinCost() const { return minBucket; }
    
    // Retorna custo máximo suportado
    int getMaxCost() const { return maxBucket; }
    
    // Verifica se custo está no range válido
    bool isValidCost(int cost) const { 
        return cost >= 0 && cost <= maxBucket; 
    }
    
    // Reset para reutilização
    void clear();
    
    // === DEBUGGING E ANÁLISE ===
    
    // Estatísticas da fila
    struct BucketStats {
        int activeBuckets;          // Buckets com elementos
        int minCost;               // Menor custo presente
        int maxCost;               // Maior custo presente
        size_t totalElements;      // Total de elementos
        double averageCost;        // Custo médio
        std::vector<int> bucketSizes; // Tamanho de cada bucket ativo
    };
    
    BucketStats getStatistics() const;
    void printStatistics() const;
    
    // Distribução de custos
    void printDistribution() const;
};

// === BUCKET QUEUE COM SUPORTE A FLOAT ===

// Versão que discretiza custos reais para usar bucket queue
class DiscretizedBucketQueue {
private:
    BucketQueue bucketQueue;
    double discretizationFactor;  // Fator para conversão float->int
    double inverseFactor;         // Fator para conversão int->float
    
public:
    // Construtor: maxCost em double, precision define granularidade
    DiscretizedBucketQueue(double maxCost, double precision = 0.1);
    
    // Operações com custos em double
    void push(const Pixel& pixel, double cost);
    Pixel pop();
    bool empty() const { return bucketQueue.empty(); }
    size_t size() const { return bucketQueue.size(); }
    
    // Conversões
    int discretize(double cost) const;
    double continuize(int discreteCost) const;
    
    double getMinCost() const { 
        return continuize(bucketQueue.getMinCost()); 
    }
};

// === PRIORITY QUEUE HÍBRIDA ===

// Combina bucket queue para custos baixos e heap para custos altos
// Otimização para casos com mix de custos inteiros e reais
class HybridPriorityQueue {
private:
    BucketQueue bucketQueue;
    std::priority_queue<std::pair<double, Pixel>, 
                       std::vector<std::pair<double, Pixel>>,
                       std::greater<std::pair<double, Pixel>>> heap;
    
    double bucketThreshold;  // Threshold para usar bucket vs heap
    
public:
    HybridPriorityQueue(int maxBucketCost, double threshold);
    
    void push(const Pixel& pixel, double cost);
    Pixel pop();
    bool empty() const;
    size_t size() const;
    
    // Estatísticas de uso
    struct HybridStats {
        size_t bucketElements;
        size_t heapElements;
        double bucketRatio;
    };
    
    HybridStats getUsageStats() const;
};

// === FUNÇÕES AUXILIARES ===

// Factory para criar bucket queue otimizada baseada nas características dos dados
std::unique_ptr<BucketQueue> createOptimalBucketQueue(
    const class Image& image, 
    const class PathCostFunction& costFunc,
    int maxCostHint = -1
);

// Benchmark entre diferentes implementações de priority queue
struct PriorityQueueBenchmark {
    double bucketQueueTimeMs;
    double stdPriorityQueueTimeMs;
    double hybridQueueTimeMs;
    size_t operationsCount;
    
    void print() const;
    double getBestTimeMs() const;
    std::string getBestImplementation() const;
};

PriorityQueueBenchmark benchmarkPriorityQueues(
    const std::vector<std::pair<Pixel, int>>& operations
);

#endif 