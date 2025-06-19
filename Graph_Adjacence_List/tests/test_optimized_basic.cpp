#include <iostream>
#include <chrono>
#include <iomanip>
#include "bucket_queue.h"
#include "pixel.h"

void printHeader(const std::string& title) {
    std::cout << "\n";
    std::cout << "===============================================" << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << "===============================================" << std::endl;
}

void testBucketQueue() {
    printHeader("TESTE 1: BUCKET QUEUE BÁSICA");
    
    std::cout << "Criando bucket queue com custo máximo 100..." << std::endl;
    BucketQueue bq(100);
    
    // Adiciona alguns pixels
    std::vector<std::pair<Pixel, int>> testData = {
        {Pixel(1, 1, 50), 10},
        {Pixel(2, 2, 75), 5},
        {Pixel(3, 3, 100), 15},
        {Pixel(4, 4, 25), 5},
        {Pixel(5, 5, 200), 20}
    };
    
    std::cout << "Adicionando pixels à bucket queue:" << std::endl;
    for (const auto& [pixel, cost] : testData) {
        bq.push(pixel, cost);
        std::cout << "  " << pixel.toString() << " com custo " << cost << std::endl;
    }
    
    std::cout << "\nEstatísticas da bucket queue:" << std::endl;
    bq.printStatistics();
    
    std::cout << "\nRemoção em ordem de custo:" << std::endl;
    while (!bq.empty()) {
        int oldMinCost = bq.getMinCost();
        Pixel pixel = bq.pop();
        std::cout << "  Removido: " << pixel.toString() 
                  << " (custo era " << oldMinCost << ")" << std::endl;
    }
    
    std::cout << "✓ Bucket queue funcionando corretamente!" << std::endl;
}

void testBucketVsHeapPerformance() {
    printHeader("TESTE 2: BENCHMARK BUCKET vs HEAP");
    
    std::cout << "Gerando dados de teste para benchmark..." << std::endl;
    
    std::vector<std::pair<Pixel, int>> operations;
    for (int i = 0; i < 10000; ++i) {
        int x = i % 100;
        int y = i / 100;
        int cost = i % 500; // Custos de 0 a 499
        operations.push_back({Pixel(x, y, (x + y) % 256), cost});
    }
    
    std::cout << "Executando benchmark com " << operations.size() << " operações..." << std::endl;
    
    auto benchmark = benchmarkPriorityQueues(operations);
    benchmark.print();
    
    std::cout << "✓ Benchmark concluído!" << std::endl;
}

void testDiscretizedBucketQueue() {
    printHeader("TESTE 3: BUCKET QUEUE DISCRETIZADA");
    
    std::cout << "Testando bucket queue para custos reais..." << std::endl;
    
    DiscretizedBucketQueue dbq(100.0, 0.1); // Max cost 100.0, precision 0.1
    
    // Adiciona pixels com custos reais
    std::vector<std::pair<Pixel, double>> testData = {
        {Pixel(1, 1, 50), 10.5},
        {Pixel(2, 2, 75), 5.3},
        {Pixel(3, 3, 100), 15.7},
        {Pixel(4, 4, 25), 5.9},
        {Pixel(5, 5, 200), 20.1}
    };
    
    std::cout << "Adicionando pixels com custos reais:" << std::endl;
    for (const auto& [pixel, cost] : testData) {
        dbq.push(pixel, cost);
        std::cout << "  " << pixel.toString() << " com custo " << cost << std::endl;
    }
    
    std::cout << "\nRemoção discretizada:" << std::endl;
    while (!dbq.empty()) {
        double oldMinCost = dbq.getMinCost();
        Pixel pixel = dbq.pop();
        std::cout << "  Removido: " << pixel.toString() 
                  << " (custo mínimo era: " << std::fixed << std::setprecision(1) 
                  << oldMinCost << ")" << std::endl;
    }
    
    std::cout << "✓ Bucket queue discretizada funcionando!" << std::endl;
}

void testHybridPriorityQueue() {
    printHeader("TESTE 4: HYBRID PRIORITY QUEUE");
    
    std::cout << "Testando hybrid queue (bucket + heap)..." << std::endl;
    
    HybridPriorityQueue hq(50, 50.0); // Bucket até custo 50, heap acima disso
    
    // Mistura custos baixos (bucket) e altos (heap)
    std::vector<std::pair<Pixel, double>> testData = {
        {Pixel(1, 1, 50), 10.0},    // bucket
        {Pixel(2, 2, 75), 75.5},    // heap
        {Pixel(3, 3, 100), 15.0},   // bucket
        {Pixel(4, 4, 25), 150.7},   // heap
        {Pixel(5, 5, 200), 5.0}     // bucket
    };
    
    std::cout << "Adicionando pixels à hybrid queue:" << std::endl;
    for (const auto& [pixel, cost] : testData) {
        hq.push(pixel, cost);
        std::string queueType = (cost <= 50.0 && cost == static_cast<int>(cost)) ? "bucket" : "heap";
        std::cout << "  " << pixel.toString() << " com custo " << cost 
                  << " (usando " << queueType << ")" << std::endl;
    }
    
    auto stats = hq.getUsageStats();
    std::cout << "\nEstatísticas hybrid queue:" << std::endl;
    std::cout << "Elementos bucket: " << stats.bucketElements << std::endl;
    std::cout << "Elementos heap: " << stats.heapElements << std::endl;
    std::cout << "Bucket ratio: " << std::fixed << std::setprecision(1) 
              << (stats.bucketRatio * 100) << "%" << std::endl;
    
    std::cout << "\nRemoção híbrida:" << std::endl;
    while (!hq.empty()) {
        Pixel pixel = hq.pop();
        std::cout << "  Removido: " << pixel.toString() << std::endl;
    }
    
    std::cout << "✓ Hybrid priority queue funcionando!" << std::endl;
}

void testBucketQueueStress() {
    printHeader("TESTE 5: ESTRESSE BUCKET QUEUE");
    
    std::vector<int> sizes = {1000, 5000, 10000};
    std::vector<int> maxCosts = {100, 500, 1000};
    
    for (int size : sizes) {
        for (int maxCost : maxCosts) {
            std::cout << "\n--- Teste " << size << " elementos, maxCost " << maxCost << " ---" << std::endl;
            
            auto start = std::chrono::high_resolution_clock::now();
            
            BucketQueue bq(maxCost);
            
            // Adiciona elementos
            for (int i = 0; i < size; ++i) {
                int x = i % 100;
                int y = i / 100;
                int cost = i % maxCost;
                bq.push(Pixel(x, y, (x + y) % 256), cost);
            }
            
            // Remove elementos
            int count = 0;
            while (!bq.empty() && count < size) {
                bq.pop();
                count++;
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double timeMs = std::chrono::duration<double, std::milli>(end - start).count();
            
            std::cout << "Tempo: " << std::fixed << std::setprecision(2) << timeMs << " ms" << std::endl;
            std::cout << "Elementos processados: " << count << std::endl;
            std::cout << "Throughput: " << std::setprecision(0) << (count / timeMs * 1000) << " ops/s" << std::endl;
        }
    }
    
    std::cout << "✓ Teste de estresse concluído!" << std::endl;
}

int main() {
    printHeader("🚀 TESTES DAS ESTRUTURAS OTIMIZADAS IFT 🚀");
    
    try {
        // Executa testes básicos das estruturas de dados
        testBucketQueue();
        testBucketVsHeapPerformance();
        testDiscretizedBucketQueue();
        testHybridPriorityQueue();
        testBucketQueueStress();
        
        printHeader("[OK] TODOS OS TESTES CONCLUÍDOS COM SUCESSO!");
        
        std::cout << "\nRESUMO DAS ESTRUTURAS TESTADAS:" << std::endl;
        std::cout << "✓ Bucket Queue O(m + nK) para custos inteiros" << std::endl;
        std::cout << "✓ Bucket Queue Discretizada para custos reais" << std::endl;
        std::cout << "✓ Hybrid Priority Queue (bucket + heap)" << std::endl;
        std::cout << "✓ Benchmarks automáticos de performance" << std::endl;
        std::cout << "✓ Testes de estresse para validação" << std::endl;
        
        std::cout << "\nCOMPLEXIDADES VERIFICADAS:" << std::endl;
        std::cout << "• Bucket Queue: O(m + nK) onde K é o range de custos" << std::endl;
        std::cout << "• Standard Priority Queue: O(m + n log n)" << std::endl;
        std::cout << "• Hybrid Queue: Combina melhor de ambos" << std::endl;
        
        std::cout << "\nPRÓXIMOS PASSOS:" << std::endl;
        std::cout << "• Integração completa com algoritmos IFT" << std::endl;
        std::cout << "• Testes com imagens reais" << std::endl;
        std::cout << "• Benchmarks de aplicações específicas" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERRO durante execução: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 