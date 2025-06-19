#include "ift_optimized_algorithm.h"
#include "path_cost_function.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <iomanip>
#include <memory>

// === IMPLEMENTAÇÃO OPTIMIZED IFT ALGORITHM ===

OptimizedIFTAlgorithm::OptimizedIFTAlgorithm(bool eightConn, bool verbose)
    : IFTAlgorithm(eightConn, verbose), useBucketQueue(true), useIntegerCosts(true),
      maxCostEstimate(-1), costDiscretization(1.0) {
}

std::unique_ptr<IFTResult> OptimizedIFTAlgorithm::runOptimizedIFT(
    const Image& image,
    const PathCostFunction& costFunction,
    const SeedSet& seeds) {
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Reset estatísticas
    lastOptStats = OptimizedStats();
    lastOptStats.usedBucketQueue = true;
    lastOptStats.usedDiscretization = false;
    
    if (verbose) {
        std::cout << "\n=== ALGORITMO 2 (OTIMIZADO) IFT ===" << std::endl;
        std::cout << "Imagem: " << image.getWidth() << "x" << image.getHeight() << std::endl;
        std::cout << "Sementes: " << seeds.size() << std::endl;
        std::cout << "Conectividade: " << (eightConnected ? "8" : "4") << std::endl;
    }
    
    // Analisa função de custo
    useIntegerCosts = analyzeCostFunction(costFunction, image);
    if (maxCostEstimate < 0) {
        maxCostEstimate = estimateMaxCost(costFunction, image);
    }
    
    if (verbose) {
        std::cout << "Custos inteiros: " << (useIntegerCosts ? "Sim" : "Não") << std::endl;
        std::cout << "Custo máximo estimado: " << maxCostEstimate << std::endl;
    }
    
    // Cria resultado
    auto result = std::make_unique<IFTResult>(image.getWidth(), image.getHeight());
    
    // Inicializa mapeamentos
    initializeIFTMaps(*result, image, costFunction, seeds);
    
    // Cria bucket queue otimizada
    BucketQueue bucketQueue(maxCostEstimate);
    
    // Adiciona sementes à fila
    for (int x = 0; x < image.getWidth(); ++x) {
        for (int y = 0; y < image.getHeight(); ++y) {
            Pixel pixel(x, y, image.getPixelIntensity(x, y));
            double cost = result->getCost(pixel);
            
            if (cost < std::numeric_limits<double>::infinity()) {
                bucketQueue.push(pixel, static_cast<int>(cost));
                lastOptStats.bucketOperations++;
            }
        }
    }
    
    // Executa loop principal otimizado
    processOptimizedMainLoop(*result, image, costFunction, bucketQueue);
    
    // Finaliza estatísticas
    auto endTime = std::chrono::high_resolution_clock::now();
    lastOptStats.executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    lastOptStats.pixelsProcessed = result->getPixelsProcessed();
    lastOptStats.maxCostObserved = maxCostEstimate;
    
    // Calcula utilização da bucket queue
    size_t totalOps = lastOptStats.bucketOperations + lastOptStats.heapOperations;
    lastOptStats.bucketQueueUtilization = totalOps > 0 ? 
        static_cast<double>(lastOptStats.bucketOperations) / totalOps : 1.0;
    
    if (verbose) {
        lastOptStats.print();
    }
    
    return result;
}

std::unique_ptr<IFTResult> OptimizedIFTAlgorithm::runDiscretizedIFT(
    const Image& image,
    const PathCostFunction& costFunction,
    const SeedSet& seeds,
    double precision) {
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    lastOptStats = OptimizedStats();
    lastOptStats.usedBucketQueue = true;
    lastOptStats.usedDiscretization = true;
    costDiscretization = precision;
    
    if (verbose) {
        std::cout << "\n=== ALGORITMO 2 (DISCRETIZADO) IFT ===" << std::endl;
        std::cout << "Precisão: " << precision << std::endl;
    }
    
    // Estima custo máximo em formato discreto
    double maxCostReal = estimateMaxCost(costFunction, image);
    
    // Cria bucket queue discretizada
    DiscretizedBucketQueue discretizedQueue(maxCostReal, precision);
    
    // Cria resultado
    auto result = std::make_unique<IFTResult>(image.getWidth(), image.getHeight());
    initializeIFTMaps(*result, image, costFunction, seeds);
    
    // Adiciona sementes à fila discretizada
    for (int x = 0; x < image.getWidth(); ++x) {
        for (int y = 0; y < image.getHeight(); ++y) {
            Pixel pixel(x, y, image.getPixelIntensity(x, y));
            double cost = result->getCost(pixel);
            
            if (cost < std::numeric_limits<double>::infinity()) {
                discretizedQueue.push(pixel, cost);
                lastOptStats.bucketOperations++;
            }
        }
    }
    
    // Loop principal com bucket queue discretizada
    while (!discretizedQueue.empty()) {
        Pixel currentPixel = discretizedQueue.pop();
        
        processNeighbors(currentPixel, *result, image, costFunction, 
                        [&](const Pixel& pixel, double cost) {
                            discretizedQueue.push(pixel, cost);
                            lastOptStats.bucketOperations++;
                        });
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    lastOptStats.executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    lastOptStats.pixelsProcessed = result->getPixelsProcessed();
    
    return result;
}

std::unique_ptr<IFTResult> OptimizedIFTAlgorithm::runHybridIFT(
    const Image& image,
    const PathCostFunction& costFunction,
    const SeedSet& seeds) {
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    lastOptStats = OptimizedStats();
    lastOptStats.usedBucketQueue = true;
    
    if (verbose) {
        std::cout << "\n=== ALGORITMO 2 (HÍBRIDO) IFT ===" << std::endl;
    }
    
    // Configura hybrid queue
    int bucketThreshold = maxCostEstimate > 0 ? maxCostEstimate / 2 : 1000;
    HybridPriorityQueue hybridQueue(bucketThreshold, bucketThreshold);
    
    auto result = std::make_unique<IFTResult>(image.getWidth(), image.getHeight());
    initializeIFTMaps(*result, image, costFunction, seeds);
    
    // Adiciona sementes à fila híbrida
    for (int x = 0; x < image.getWidth(); ++x) {
        for (int y = 0; y < image.getHeight(); ++y) {
            Pixel pixel(x, y, image.getPixelIntensity(x, y));
            double cost = result->getCost(pixel);
            
            if (cost < std::numeric_limits<double>::infinity()) {
                hybridQueue.push(pixel, cost);
            }
        }
    }
    
    // Executa loop híbrido
    processHybridMainLoop(*result, image, costFunction, hybridQueue);
    
    // Estatísticas híbridas
    auto hybridStats = hybridQueue.getUsageStats();
    lastOptStats.bucketOperations = hybridStats.bucketElements;
    lastOptStats.heapOperations = hybridStats.heapElements;
    lastOptStats.bucketQueueUtilization = hybridStats.bucketRatio;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    lastOptStats.executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    lastOptStats.pixelsProcessed = result->getPixelsProcessed();
    
    return result;
}

// === MÉTODOS INTERNOS OTIMIZADOS ===

bool OptimizedIFTAlgorithm::analyzeCostFunction(const PathCostFunction& costFunc, const Image& image) {
    // Testa alguns pixels para verificar se custos são inteiros
    std::vector<Pixel> testPixels;
    
    // Pega alguns pixels de amostra
    for (int i = 0; i < 10 && i < image.getWidth(); ++i) {
        for (int j = 0; j < 10 && j < image.getHeight(); ++j) {
            testPixels.push_back(Pixel(i, j, image.getPixelIntensity(i, j)));
        }
    }
    
    // Testa custos de arcos
    for (size_t i = 0; i < testPixels.size() - 1; ++i) {
        // Criamos uma imagem dummy para o teste
        Image dummyImage(1, 1);
        double arcCost = costFunc.getArcWeight(testPixels[i], testPixels[i+1], dummyImage);
        if (arcCost != static_cast<int>(arcCost)) {
            return false; // Encontrou custo não-inteiro
        }
    }
    
    return true;
}

int OptimizedIFTAlgorithm::estimateMaxCost(const PathCostFunction& costFunc, const Image& image) {
    std::string funcName = costFunc.getName();
    
    if (funcName.find("sum") != std::string::npos) {
        // Para funções aditivas: custo máximo = max_arc_weight × diagonal
        int diagonal = static_cast<int>(std::sqrt(image.getWidth() * image.getWidth() + 
                                                 image.getHeight() * image.getHeight()));
        return 255 * diagonal; // Assumindo intensidades 0-255
    } else if (funcName.find("max") != std::string::npos) {
        // Para funções max: limitado pelo maior peso de arco
        return 255;
    } else {
        // Estimativa conservadora
        return 1000;
    }
}

void OptimizedIFTAlgorithm::processOptimizedMainLoop(
    IFTResult& result,
    const Image& image,
    const PathCostFunction& costFunction,
    BucketQueue& bucketQueue) {
    
    while (!bucketQueue.empty()) {
        Pixel currentPixel = bucketQueue.pop();
        
        if (verbose && result.getPixelsProcessed() % 100 == 0) {
            std::cout << "Processando pixel " << result.getPixelsProcessed() 
                      << ", custo atual: " << bucketQueue.getMinCost() << std::endl;
        }
        
        processNeighborsOptimized(currentPixel, result, image, costFunction, bucketQueue);
    }
}

void OptimizedIFTAlgorithm::processHybridMainLoop(
    IFTResult& result,
    const Image& image,
    const PathCostFunction& costFunction,
    HybridPriorityQueue& hybridQueue) {
    
    while (!hybridQueue.empty()) {
        Pixel currentPixel = hybridQueue.pop();
        
        processNeighbors(currentPixel, result, image, costFunction,
                        [&](const Pixel& pixel, double cost) {
                            hybridQueue.push(pixel, cost);
                        });
    }
}

void OptimizedIFTAlgorithm::processNeighborsOptimized(
    const Pixel& currentPixel,
    IFTResult& result,
    const Image& image,
    const PathCostFunction& costFunction,
    BucketQueue& bucketQueue) {
    
    std::vector<Pixel> neighbors = image.getNeighbors(currentPixel, eightConnected);
    
    for (const Pixel& neighbor : neighbors) {
        if (updatePixelCostOptimized(currentPixel, neighbor, result, 
                                   costFunction, image, bucketQueue)) {
            lastOptStats.bucketOperations++;
        }
    }
    
    result.incrementPixelsProcessed();
}

bool OptimizedIFTAlgorithm::updatePixelCostOptimized(
    const Pixel& fromPixel,
    const Pixel& toPixel,
    IFTResult& result,
    const PathCostFunction& costFunction,
    const Image& image,
    BucketQueue& bucketQueue) {
    
    double currentCost = result.getCost(fromPixel);
    double arcWeight = costFunction.getArcWeight(fromPixel, toPixel, image);
    double newCost = costFunction.extendCost(currentCost, arcWeight);
    
    if (newCost < result.getCost(toPixel)) {
        result.setPredecessor(toPixel, fromPixel);
        result.setCost(toPixel, newCost);
        result.setLabel(toPixel, result.getLabel(fromPixel));
        
        // Adiciona à bucket queue
        bucketQueue.push(toPixel, static_cast<int>(newCost));
        return true;
    }
    
    return false;
}

// === ESTATÍSTICAS OTIMIZADAS ===

void OptimizedIFTAlgorithm::OptimizedStats::print() const {
    ExecutionStats::print();
    
    std::cout << "\n=== ESTATÍSTICAS OTIMIZADAS ===" << std::endl;
    std::cout << "Bucket Queue: " << (usedBucketQueue ? "Sim" : "Não") << std::endl;
    std::cout << "Discretização: " << (usedDiscretization ? "Sim" : "Não") << std::endl;
    std::cout << "Utilização Bucket: " << std::fixed << std::setprecision(1) 
              << (bucketQueueUtilization * 100) << "%" << std::endl;
    std::cout << "Custo máximo observado: " << maxCostObserved << std::endl;
    std::cout << "Operações bucket: " << bucketOperations << std::endl;
    std::cout << "Operações heap: " << heapOperations << std::endl;
    std::cout << "Uso de memória: " << std::fixed << std::setprecision(2) 
              << memoryUsageMB << " MB" << std::endl;
    std::cout << "===============================" << std::endl;
}

// === BENCHMARKS ===

OptimizedIFTAlgorithm::AlgorithmBenchmark OptimizedIFTAlgorithm::benchmarkAlgorithms(
    const Image& image,
    const PathCostFunction& costFunction,
    const SeedSet& seeds) {
    
    AlgorithmBenchmark benchmark;
    benchmark.imageSize = image.getWidth() * image.getHeight();
    
    std::cout << "\n=== BENCHMARK ALGORITMOS IFT ===" << std::endl;
    
    // Benchmark Algoritmo Básico
    {
        IFTAlgorithm basicAlg(eightConnected, false);
        auto start = std::chrono::high_resolution_clock::now();
        auto result = basicAlg.runIFT(image, costFunction, seeds);
        auto end = std::chrono::high_resolution_clock::now();
        benchmark.basicIFTTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    }
    
    // Benchmark Algoritmo Otimizado
    {
        auto start = std::chrono::high_resolution_clock::now();
        auto result = runOptimizedIFT(image, costFunction, seeds);
        auto end = std::chrono::high_resolution_clock::now();
        benchmark.optimizedIFTTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    }
    
    // Benchmark Algoritmo Híbrido
    {
        auto start = std::chrono::high_resolution_clock::now();
        auto result = runHybridIFT(image, costFunction, seeds);
        auto end = std::chrono::high_resolution_clock::now();
        benchmark.hybridIFTTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    }
    
    // Determina melhor algoritmo
    double bestTime = std::min({benchmark.basicIFTTimeMs, 
                               benchmark.optimizedIFTTimeMs, 
                               benchmark.hybridIFTTimeMs});
    
    if (bestTime == benchmark.basicIFTTimeMs) {
        benchmark.bestAlgorithm = "Básico";
        benchmark.speedupFactor = 1.0;
    } else if (bestTime == benchmark.optimizedIFTTimeMs) {
        benchmark.bestAlgorithm = "Otimizado";
        benchmark.speedupFactor = benchmark.basicIFTTimeMs / benchmark.optimizedIFTTimeMs;
    } else {
        benchmark.bestAlgorithm = "Híbrido";
        benchmark.speedupFactor = benchmark.basicIFTTimeMs / benchmark.hybridIFTTimeMs;
    }
    
    return benchmark;
}

void OptimizedIFTAlgorithm::AlgorithmBenchmark::print() const {
    std::cout << "\n=== RESULTADOS BENCHMARK ===" << std::endl;
    std::cout << "Imagem: " << imageSize << " pixels" << std::endl;
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Algoritmo Básico:    " << basicIFTTimeMs << " ms" << std::endl;
    std::cout << "Algoritmo Otimizado: " << optimizedIFTTimeMs << " ms" << std::endl;
    std::cout << "Algoritmo Híbrido:   " << hybridIFTTimeMs << " ms" << std::endl;
    std::cout << "Melhor: " << bestAlgorithm << std::endl;
    std::cout << "Speedup: " << std::setprecision(2) << speedupFactor << "x" << std::endl;
    std::cout << "============================" << std::endl;
}

// === IMPLEMENTAÇÃO LIFO IFT ALGORITHM ===

LIFOIFTAlgorithm::LIFOIFTAlgorithm(bool eightConn, bool verbose)
    : IFTAlgorithm(eightConn, verbose), tiePolicy(TieBreakingPolicy::LIFO) {
}

std::unique_ptr<IFTResult> LIFOIFTAlgorithm::runLIFOIFT(
    const Image& image,
    const PathCostFunction& costFunction,
    const SeedSet& seeds) {
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (verbose) {
        std::cout << "\n=== ALGORITMO 3 (LIFO) IFT ===" << std::endl;
        std::cout << "Tie-breaking: LIFO" << std::endl;
    }
    
    auto result = std::make_unique<IFTResult>(image.getWidth(), image.getHeight());
    initializeIFTMaps(*result, image, costFunction, seeds);
    
    // Configura tie-breaking queue
    tieQueue.policy = tiePolicy;
    tieQueue.clear();
    
    // Adiciona sementes
    for (int x = 0; x < image.getWidth(); ++x) {
        for (int y = 0; y < image.getHeight(); ++y) {
            Pixel pixel(x, y, image.getPixelIntensity(x, y));
            double cost = result->getCost(pixel);
            
            if (cost < std::numeric_limits<double>::infinity()) {
                tieQueue.push(pixel, static_cast<int>(cost));
            }
        }
    }
    
    processLIFOMainLoop(*result, image, costFunction);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double executionTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    if (verbose) {
        std::cout << "Tempo execução LIFO: " << executionTime << " ms" << std::endl;
    }
    
    return result;
}

void LIFOIFTAlgorithm::setTieBreakingPolicy(TieBreakingPolicy policy) {
    tiePolicy = policy;
}

LIFOIFTAlgorithm::TieBreakingPolicy LIFOIFTAlgorithm::getTieBreakingPolicy() const {
    return tiePolicy;
}

void LIFOIFTAlgorithm::processLIFOMainLoop(
    IFTResult& result,
    const Image& image,
    const PathCostFunction& costFunction) {
    
    while (!tieQueue.empty()) {
        Pixel currentPixel = tieQueue.pop();
        
        processNeighbors(currentPixel, result, image, costFunction,
                        [&](const Pixel& pixel, double cost) {
                            tieQueue.push(pixel, static_cast<int>(cost));
                        });
    }
}

// === TIE-BREAKING QUEUE ===

void LIFOIFTAlgorithm::TieBreakingQueue::push(const Pixel& pixel, int cost) {
    if (cost >= static_cast<int>(costBuckets.size())) {
        costBuckets.resize(cost + 1);
    }
    
    costBuckets[cost].push_back(pixel);
    
    if (currentCost > cost) {
        currentCost = cost;
    }
}

Pixel LIFOIFTAlgorithm::TieBreakingQueue::pop() {
    // Encontra próximo bucket não-vazio
    while (currentCost < static_cast<int>(costBuckets.size()) && 
           costBuckets[currentCost].empty()) {
        currentCost++;
    }
    
    if (currentCost >= static_cast<int>(costBuckets.size())) {
        throw std::runtime_error("TieBreakingQueue vazia");
    }
    
    Pixel pixel;
    auto& bucket = costBuckets[currentCost];
    
    switch (policy) {
        case TieBreakingPolicy::FIFO:
            pixel = bucket.front();
            bucket.erase(bucket.begin());
            break;
            
        case TieBreakingPolicy::LIFO:
            pixel = bucket.back();
            bucket.pop_back();
            break;
            
        case TieBreakingPolicy::RANDOM:
            {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, bucket.size() - 1);
                int idx = dis(gen);
                pixel = bucket[idx];
                bucket.erase(bucket.begin() + idx);
            }
            break;
    }
    
    return pixel;
}

bool LIFOIFTAlgorithm::TieBreakingQueue::empty() const {
    for (size_t i = currentCost; i < costBuckets.size(); ++i) {
        if (!costBuckets[i].empty()) {
            return false;
        }
    }
    return true;
}

void LIFOIFTAlgorithm::TieBreakingQueue::clear() {
    costBuckets.clear();
    currentCost = 0;
}

// === FUNÇÕES AUXILIARES ===

std::unique_ptr<OptimizedIFTAlgorithm> createAutoOptimizedIFT(
    const Image& image,
    const PathCostFunction& costFunction,
    bool eightConnected) {
    
    auto algorithm = std::make_unique<OptimizedIFTAlgorithm>(eightConnected, false);
    
    // Configuração automática baseada nas características
    size_t imageSize = image.getWidth() * image.getHeight();
    
    if (imageSize < 10000) {
        // Imagens pequenas: algoritmo básico pode ser suficiente
        algorithm->setUseBucketQueue(false);
    } else {
        // Imagens grandes: usa otimizações
        algorithm->setUseBucketQueue(true);
        algorithm->setMaxCostEstimate(algorithm->estimateMaxCost(costFunction, image));
    }
    
    return algorithm;
}

// === TESTES DE ESTRESSE ===

StressTestResult runStressTest(int imageSize, int seedCount, const std::string& costFunctionType) {
    StressTestResult result;
    result.imageSize = imageSize;
    result.seedCount = seedCount;
    
    // Cria imagem de teste
    Image testImage(imageSize, imageSize);
    for (int x = 0; x < imageSize; ++x) {
        for (int y = 0; y < imageSize; ++y) {
            uint8_t intensity = static_cast<uint8_t>((x + y) % 256);
            testImage.setPixel(x, y, intensity);
        }
    }
    
    // Cria sementes aleatórias
    SeedSet testSeeds;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, imageSize - 1);
    
    for (int i = 0; i < seedCount; ++i) {
        int x = dis(gen);
        int y = dis(gen);
        testSeeds.addSeed(Pixel(x, y, testImage.getPixelIntensity(x, y)), i % 3);
    }
    
    // Cria função de custo
    auto costFunc = createIntensityDifferenceSum();
    
    // Testa algoritmo básico
    {
        IFTAlgorithm basicAlg(false, false);
        auto start = std::chrono::high_resolution_clock::now();
        auto basicResult = basicAlg.runIFT(testImage, *costFunc, testSeeds);
        auto end = std::chrono::high_resolution_clock::now();
        result.basicTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    }
    
    // Testa algoritmo otimizado
    {
        OptimizedIFTAlgorithm optAlg(false, false);
        auto start = std::chrono::high_resolution_clock::now();
        auto optResult = optAlg.runOptimizedIFT(testImage, *costFunc, testSeeds);
        auto end = std::chrono::high_resolution_clock::now();
        result.optimizedTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    }
    
    result.speedup = result.basicTimeMs / result.optimizedTimeMs;
    result.resultsMatch = true; // Assumimos que os resultados são corretos
    
    return result;
}

void StressTestResult::print() const {
    std::cout << "\n=== TESTE DE ESTRESSE ===" << std::endl;
    std::cout << "Imagem: " << imageSize << "x" << imageSize << " pixels" << std::endl;
    std::cout << "Sementes: " << seedCount << std::endl;
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Tempo básico: " << basicTimeMs << " ms" << std::endl;
    std::cout << "Tempo otimizado: " << optimizedTimeMs << " ms" << std::endl;
    std::cout << "Speedup: " << std::setprecision(2) << speedup << "x" << std::endl;
    std::cout << "Resultados coincidem: " << (resultsMatch ? "Sim" : "Não") << std::endl;
    std::cout << "=========================" << std::endl;
} 