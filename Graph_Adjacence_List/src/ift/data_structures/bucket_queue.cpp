#include "bucket_queue.h"
#include "image.h"
#include "path_cost_function.h"
#include <algorithm>
#include <iomanip>
#include <chrono>

// === IMPLEMENTAÇÃO DA BUCKET QUEUE ===

BucketQueue::BucketQueue(int maxCost) 
    : buckets(maxCost + 1), minBucket(maxCost + 1), maxBucket(maxCost), 
      bucketCount(maxCost + 1), totalElements(0) {
    // Inicializa com buckets vazios
}

void BucketQueue::push(const Pixel& pixel, int cost) {
    if (!isValidCost(cost)) {
        std::cerr << "ERRO: Custo " << cost << " fora do range [0, " << maxBucket << "]" << std::endl;
        return;
    }
    
    buckets[cost].push(pixel);
    totalElements++;
    
    // Atualiza minBucket se necessário
    if (cost < minBucket) {
        minBucket = cost;
    }
}

Pixel BucketQueue::pop() {
    if (empty()) {
        throw std::runtime_error("BucketQueue::pop() chamado em fila vazia");
    }
    
    // Encontra bucket não-vazio com menor custo
    updateMinBucket();
    
    if (minBucket > maxBucket) {
        throw std::runtime_error("BucketQueue: Estado inconsistente - nenhum bucket válido");
    }
    
    Pixel pixel = buckets[minBucket].front();
    buckets[minBucket].pop();
    totalElements--;
    
    return pixel;
}

const Pixel& BucketQueue::top() const {
    if (empty()) {
        throw std::runtime_error("BucketQueue::top() chamado em fila vazia");
    }
    
    // Encontra primeiro bucket não-vazio
    for (int i = minBucket; i <= maxBucket; ++i) {
        if (!buckets[i].empty()) {
            return buckets[i].front();
        }
    }
    
    throw std::runtime_error("BucketQueue: Estado inconsistente - fila não vazia mas sem elementos");
}

void BucketQueue::updateMinBucket() {
    while (minBucket <= maxBucket && buckets[minBucket].empty()) {
        minBucket++;
    }
}

void BucketQueue::clear() {
    for (auto& bucket : buckets) {
        while (!bucket.empty()) {
            bucket.pop();
        }
    }
    
    minBucket = maxBucket + 1;
    totalElements = 0;
}

BucketQueue::BucketStats BucketQueue::getStatistics() const {
    BucketStats stats;
    stats.activeBuckets = 0;
    stats.minCost = maxBucket + 1;
    stats.maxCost = -1;
    stats.totalElements = totalElements;
    
    double costSum = 0.0;
    for (int i = 0; i <= maxBucket; ++i) {
        if (!buckets[i].empty()) {
            stats.activeBuckets++;
            stats.bucketSizes.push_back(static_cast<int>(buckets[i].size()));
            
            if (i < stats.minCost) stats.minCost = i;
            if (i > stats.maxCost) stats.maxCost = i;
            
            costSum += i * buckets[i].size();
        }
    }
    
    stats.averageCost = totalElements > 0 ? costSum / totalElements : 0.0;
    
    return stats;
}

void BucketQueue::printStatistics() const {
    auto stats = getStatistics();
    
    std::cout << "=== BUCKET QUEUE STATISTICS ===" << std::endl;
    std::cout << "Total elements: " << stats.totalElements << std::endl;
    std::cout << "Active buckets: " << stats.activeBuckets << "/" << bucketCount << std::endl;
    std::cout << "Cost range: [" << stats.minCost << ", " << stats.maxCost << "]" << std::endl;
    std::cout << "Average cost: " << std::fixed << std::setprecision(2) << stats.averageCost << std::endl;
    std::cout << "==============================" << std::endl;
}

void BucketQueue::printDistribution() const {
    std::cout << "\n=== COST DISTRIBUTION ===" << std::endl;
    
    for (int i = 0; i <= maxBucket; ++i) {
        if (!buckets[i].empty()) {
            std::cout << "Bucket " << std::setw(3) << i << ": " 
                      << std::setw(4) << buckets[i].size() << " elements ";
            
            // Barra visual
            int barLength = static_cast<int>(buckets[i].size() * 50 / totalElements);
            for (int j = 0; j < barLength; ++j) {
                std::cout << "█";
            }
            std::cout << std::endl;
        }
    }
    std::cout << "=========================" << std::endl;
}

// === DISCRETIZED BUCKET QUEUE ===

DiscretizedBucketQueue::DiscretizedBucketQueue(double maxCost, double precision) 
    : bucketQueue(static_cast<int>(maxCost / precision) + 1),
      discretizationFactor(1.0 / precision),
      inverseFactor(precision) {
}

void DiscretizedBucketQueue::push(const Pixel& pixel, double cost) {
    int discreteCost = discretize(cost);
    bucketQueue.push(pixel, discreteCost);
}

Pixel DiscretizedBucketQueue::pop() {
    return bucketQueue.pop();
}

int DiscretizedBucketQueue::discretize(double cost) const {
    return static_cast<int>(cost * discretizationFactor + 0.5); // Round to nearest
}

double DiscretizedBucketQueue::continuize(int discreteCost) const {
    return discreteCost * inverseFactor;
}

// === HYBRID PRIORITY QUEUE ===

HybridPriorityQueue::HybridPriorityQueue(int maxBucketCost, double threshold)
    : bucketQueue(maxBucketCost), bucketThreshold(threshold) {
}

void HybridPriorityQueue::push(const Pixel& pixel, double cost) {
    if (cost <= bucketThreshold && cost == static_cast<int>(cost)) {
        // Usa bucket queue para custos inteiros baixos
        bucketQueue.push(pixel, static_cast<int>(cost));
    } else {
        // Usa heap para custos altos ou reais
        heap.push(std::make_pair(cost, pixel));
    }
}

Pixel HybridPriorityQueue::pop() {
    if (empty()) {
        throw std::runtime_error("HybridPriorityQueue::pop() chamado em fila vazia");
    }
    
    bool useBucket = !bucketQueue.empty();
    bool useHeap = !heap.empty();
    
    if (useBucket && useHeap) {
        // Compara custos mínimos
        double bucketMinCost = bucketQueue.getMinCost();
        double heapMinCost = heap.top().first;
        
        if (bucketMinCost <= heapMinCost) {
            return bucketQueue.pop();
        } else {
            Pixel pixel = heap.top().second;
            heap.pop();
            return pixel;
        }
    } else if (useBucket) {
        return bucketQueue.pop();
    } else {
        Pixel pixel = heap.top().second;
        heap.pop();
        return pixel;
    }
}

bool HybridPriorityQueue::empty() const {
    return bucketQueue.empty() && heap.empty();
}

size_t HybridPriorityQueue::size() const {
    return bucketQueue.size() + heap.size();
}

HybridPriorityQueue::HybridStats HybridPriorityQueue::getUsageStats() const {
    HybridStats stats;
    stats.bucketElements = bucketQueue.size();
    stats.heapElements = heap.size();
    size_t total = stats.bucketElements + stats.heapElements;
    stats.bucketRatio = total > 0 ? static_cast<double>(stats.bucketElements) / total : 0.0;
    
    return stats;
}

// === FUNÇÕES AUXILIARES ===

std::unique_ptr<BucketQueue> createOptimalBucketQueue(
    const Image& image, 
    const PathCostFunction& costFunc,
    int maxCostHint) {
    
    // Estima custo máximo se não fornecido
    if (maxCostHint < 0) {
        // Heurística baseada nas intensidades da imagem
        int maxIntensity = 255; // uint8_t max
        int diagonal = static_cast<int>(std::sqrt(image.getWidth() * image.getWidth() + 
                                                 image.getHeight() * image.getHeight()));
        
        if (costFunc.getName().find("sum") != std::string::npos) {
            // Para funções aditivas: custo pode crescer muito
            maxCostHint = maxIntensity * diagonal;
        } else {
            // Para funções max: limitado pela maior diferença
            maxCostHint = maxIntensity;
        }
    }
    
    return std::make_unique<BucketQueue>(maxCostHint);
}

PriorityQueueBenchmark benchmarkPriorityQueues(
    const std::vector<std::pair<Pixel, int>>& operations) {
    
    PriorityQueueBenchmark result;
    result.operationsCount = operations.size();
    
    if (operations.empty()) {
        result.bucketQueueTimeMs = 0;
        result.stdPriorityQueueTimeMs = 0;
        result.hybridQueueTimeMs = 0;
        return result;
    }
    
    // Encontra custo máximo para bucket queue
    int maxCost = 0;
    for (const auto& op : operations) {
        maxCost = std::max(maxCost, op.second);
    }
    
    // Benchmark Bucket Queue
    {
        auto start = std::chrono::high_resolution_clock::now();
        BucketQueue bq(maxCost);
        
        for (const auto& op : operations) {
            bq.push(op.first, op.second);
        }
        
        while (!bq.empty()) {
            bq.pop();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.bucketQueueTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    }
    
    // Benchmark Standard Priority Queue
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::priority_queue<std::pair<int, Pixel>, 
                           std::vector<std::pair<int, Pixel>>,
                           std::greater<std::pair<int, Pixel>>> pq;
        
        for (const auto& op : operations) {
            pq.push(std::make_pair(op.second, op.first));
        }
        
        while (!pq.empty()) {
            pq.pop();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.stdPriorityQueueTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    }
    
    // Benchmark Hybrid Queue
    {
        auto start = std::chrono::high_resolution_clock::now();
        HybridPriorityQueue hq(maxCost / 2, maxCost / 2.0);
        
        for (const auto& op : operations) {
            hq.push(op.first, op.second);
        }
        
        while (!hq.empty()) {
            hq.pop();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.hybridQueueTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    }
    
    return result;
}

void PriorityQueueBenchmark::print() const {
    std::cout << "\n=== PRIORITY QUEUE BENCHMARK ===" << std::endl;
    std::cout << "Operations: " << operationsCount << std::endl;
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Bucket Queue:    " << bucketQueueTimeMs << " ms" << std::endl;
    std::cout << "Std Priority Q:  " << stdPriorityQueueTimeMs << " ms" << std::endl;
    std::cout << "Hybrid Queue:    " << hybridQueueTimeMs << " ms" << std::endl;
    std::cout << "Best: " << getBestImplementation() << " (" << getBestTimeMs() << " ms)" << std::endl;
    std::cout << "================================" << std::endl;
}

double PriorityQueueBenchmark::getBestTimeMs() const {
    return std::min({bucketQueueTimeMs, stdPriorityQueueTimeMs, hybridQueueTimeMs});
}

std::string PriorityQueueBenchmark::getBestImplementation() const {
    double bestTime = getBestTimeMs();
    
    if (bestTime == bucketQueueTimeMs) return "Bucket Queue";
    if (bestTime == stdPriorityQueueTimeMs) return "Standard Priority Queue";
    return "Hybrid Queue";
} 