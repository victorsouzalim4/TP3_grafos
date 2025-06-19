#include "ift_algorithm.h"
#include <chrono>
#include <iostream>
#include <iomanip>

// === IMPLEMENTAÇÃO DO ALGORITMO IFT BÁSICO ===

std::unique_ptr<IFTResult> IFTAlgorithm::runBasicIFT(
    const Image& image,
    const PathCostFunction& costFunction,
    const SeedSet& seeds) {
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Cria resultado e inicializa conforme artigo
    auto result = std::make_unique<IFTResult>(image.getWidth(), image.getHeight());
    result->initializeForProcessing(image, seeds);
    
    if (verbose) {
        std::cout << "\n=== INICIANDO ALGORITMO IFT (Algoritmo 1) ===" << std::endl;
        std::cout << "Image: " << image.getWidth() << "x" << image.getHeight() << std::endl;
        std::cout << "Seeds: " << seeds.activeCount() << std::endl;
        std::cout << "Connectivity: " << (eightConnected ? "8" : "4") << "-connected" << std::endl;
        std::cout << "Cost function: " << costFunction.getName() << std::endl;
    }
    
    // Inicializa fila de prioridade Q ← I (conforme Algoritmo 1)
    auto& costMap = result->getCostMapRef();
    PixelCostComparator comparator(&costMap);
    std::priority_queue<Pixel, std::vector<Pixel>, PixelCostComparator> queue(comparator);
    
    // Adiciona todos os pixels à fila (conforme artigo: Q ← I)
    for (int y = 0; y < image.getHeight(); ++y) {
        for (int x = 0; x < image.getWidth(); ++x) {
            Pixel pixel = image.getPixel(x, y);
            queue.push(pixel);
        }
    }
    
    if (verbose) {
        std::cout << "Fila inicializada com " << queue.size() << " pixels" << std::endl;
        std::cout << "Iniciando loop principal..." << std::endl;
    }
    
    // Executa loop principal do algoritmo
    processIFTMainLoop(*result, image, costFunction, queue);
    
    // Calcula estatísticas
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    lastStats.pixelsProcessed = result->getProcessedPixelCount();
    lastStats.iterationsTotal = image.getWidth() * image.getHeight() - queue.size();
    lastStats.executionTimeMs = static_cast<double>(duration.count());
    lastStats.averageCostPerPixel = result->getAverageCost();
    lastStats.isComplete = result->isComplete();
    lastStats.isValid = result->isValidForest();
    
    if (verbose) {
        std::cout << "=== ALGORITMO IFT CONCLUÍDO ===" << std::endl;
        lastStats.print();
        result->printStatistics();
    }
    
    return result;
}

// Loop principal do Algoritmo 1: "while Q ≠ ∅"
void IFTAlgorithm::processIFTMainLoop(
    IFTResult& result,
    const Image& image,
    const PathCostFunction& costFunction,
    std::priority_queue<Pixel, std::vector<Pixel>, PixelCostComparator>& queue) {
    
    int iteration = 0;
    
    // "while Q ≠ ∅:"
    while (!queue.empty()) {
        // "Remove t from Q such that C(t) is minimum"
        Pixel currentPixel = queue.top();
        queue.pop();
        
        // Skip pixels with infinite cost (not reachable)
        if (result.getCost(currentPixel) == std::numeric_limits<double>::infinity()) {
            continue;
        }
        
        if (verbose && (iteration % 100 == 0 || iteration < 10)) {
            printAlgorithmState(result, currentPixel, iteration);
        }
        
        // "For each u ∈ A(t):" - processa vizinhos
        processNeighbors(currentPixel, result, image, costFunction, queue);
        
        iteration++;
    }
}

// Processa vizinhos conforme: "For each u ∈ A(t)"
void IFTAlgorithm::processNeighbors(
    const Pixel& currentPixel,
    IFTResult& result,
    const Image& image,
    const PathCostFunction& costFunction,
    std::priority_queue<Pixel, std::vector<Pixel>, PixelCostComparator>& queue) {
    
    // Obtém vizinhos com conectividade escolhida
    auto neighbors = image.getNeighbors(currentPixel, eightConnected);
    
    for (const Pixel& neighbor : neighbors) {
        // Tenta atualizar custo do vizinho
        bool updated = updatePixelCost(currentPixel, neighbor, result, costFunction, image);
        
        if (updated && verbose) {
            std::cout << "  Atualizou " << neighbor.toString() 
                      << " com custo " << result.getCost(neighbor) << std::endl;
        }
    }
}

// Implementa: "tmp ← f(π_t·⟨t,u⟩); if tmp < C(u) then P(u) ← t, C(u) ← tmp, L(u) ← L(t)"
bool IFTAlgorithm::updatePixelCost(
    const Pixel& fromPixel,
    const Pixel& toPixel,
    IFTResult& result,
    const PathCostFunction& costFunction,
    const Image& image) {
    
    // Calcula tmp ← f(π_t·⟨t,u⟩)
    double currentCost = result.getCost(fromPixel);
    double arcWeight = costFunction.getArcWeight(fromPixel, toPixel, image);
    double newCost = costFunction.extendCost(currentCost, arcWeight);
    
    // if tmp < C(u) then
    if (newCost < result.getCost(toPixel)) {
        // P(u) ← t, C(u) ← tmp, L(u) ← L(t)
        result.setPredecessor(toPixel, fromPixel);
        result.setCost(toPixel, newCost);
        result.setLabel(toPixel, result.getLabel(fromPixel));
        
        return true; // Pixel foi atualizado
    }
    
    return false; // Sem atualização
}

// === VARIAÇÕES DO ALGORITMO ===

std::unique_ptr<IFTResult> IFTAlgorithm::runIFTToTarget(
    const Image& image,
    const PathCostFunction& costFunction,
    const SeedSet& seeds,
    const Pixel& target) {
    
    if (verbose) {
        std::cout << "Executando IFT com early termination para target: " 
                  << target.toString() << std::endl;
    }
    
    // Mesmo início que algoritmo básico
    auto result = std::make_unique<IFTResult>(image.getWidth(), image.getHeight());
    result->initializeForProcessing(image, seeds);
    
    auto& costMap = result->getCostMapRef();
    PixelCostComparator comparator(&costMap);
    std::priority_queue<Pixel, std::vector<Pixel>, PixelCostComparator> queue(comparator);
    
    // Adiciona todos os pixels à fila
    for (int y = 0; y < image.getHeight(); ++y) {
        for (int x = 0; x < image.getWidth(); ++x) {
            queue.push(image.getPixel(x, y));
        }
    }
    
    // Loop modificado com early termination
    while (!queue.empty()) {
        Pixel currentPixel = queue.top();
        queue.pop();
        
        if (result->getCost(currentPixel) == std::numeric_limits<double>::infinity()) {
            continue;
        }
        
        // Early termination: se processamos o target, podemos parar
        if (currentPixel == target) {
            if (verbose) {
                std::cout << "Target alcançado! Custo: " << result->getCost(target) << std::endl;
            }
            break;
        }
        
        processNeighbors(currentPixel, *result, image, costFunction, queue);
    }
    
    return result;
}

// === VALIDAÇÃO ===

bool IFTAlgorithm::validateResult(
    const IFTResult& result,
    const Image& image,
    const PathCostFunction& costFunction,
    const SeedSet& seeds) const {
    
    if (verbose) {
        std::cout << "\n=== VALIDANDO RESULTADO IFT ===" << std::endl;
    }
    
    // 1. Verifica se é floresta válida (sem ciclos)
    if (!result.isValidForest()) {
        if (verbose) std::cout << "ERRO: Resultado contém ciclos!" << std::endl;
        return false;
    }
    
    // 2. Verifica se sementes têm custo correto
    auto activeSeeds = seeds.getActiveSeeds();
    for (const auto& seed : activeSeeds) {
        double expectedCost = seed.handicap;
        double actualCost = result.getCost(seed.pixel);
        
        if (std::abs(actualCost - expectedCost) > 1e-6) {
            if (verbose) {
                std::cout << "ERRO: Semente " << seed.pixel.toString() 
                          << " tem custo " << actualCost 
                          << " mas esperado " << expectedCost << std::endl;
            }
            return false;
        }
    }
    
    // 3. Verifica otimalidade de caminhos (amostragem)
    int checkCount = 0;
    for (int y = 0; y < image.getHeight() && checkCount < 100; y += 2) {
        for (int x = 0; x < image.getWidth() && checkCount < 100; x += 2) {
            Pixel pixel = image.getPixel(x, y);
            
            if (result.hasPredecessor(pixel)) {
                auto path = result.getOptimalPath(pixel);
                double pathCost = costFunction.computePathCost(path, image, seeds);
                double resultCost = result.getCost(pixel);
                
                if (std::abs(pathCost - resultCost) > 1e-6) {
                    if (verbose) {
                        std::cout << "ERRO: Custo inconsistente para " << pixel.toString()
                                  << " (caminho=" << pathCost << ", resultado=" << resultCost << ")" << std::endl;
                    }
                    return false;
                }
                checkCount++;
            }
        }
    }
    
    if (verbose) {
        std::cout << "Validação PASSOU! (" << checkCount << " pixels verificados)" << std::endl;
    }
    
    return true;
}

// === DEBUG ===

void IFTAlgorithm::printAlgorithmState(
    const IFTResult& result,
    const Pixel& currentPixel,
    int iteration) const {
    
    std::cout << "[" << std::setw(4) << iteration << "] Processando " 
              << currentPixel.toString() 
              << " (custo=" << std::fixed << std::setprecision(2) << result.getCost(currentPixel) 
              << ", label=" << result.getLabel(currentPixel) << ")" << std::endl;
}

void IFTAlgorithm::ExecutionStats::print() const {
    std::cout << "\n--- Estatísticas de Execução ---" << std::endl;
    std::cout << "Pixels processados: " << pixelsProcessed << std::endl;
    std::cout << "Iterações totais: " << iterationsTotal << std::endl;
    std::cout << "Tempo de execução: " << executionTimeMs << " ms" << std::endl;
    std::cout << "Custo médio por pixel: " << std::fixed << std::setprecision(2) << averageCostPerPixel << std::endl;
    std::cout << "Resultado completo: " << (isComplete ? "Sim" : "Não") << std::endl;
    std::cout << "Resultado válido: " << (isValid ? "Sim" : "Não") << std::endl;
    std::cout << "--------------------------------" << std::endl;
}

// === FUNÇÕES UTILITÁRIAS ===

std::unique_ptr<IFTAlgorithm> createStandardIFT(bool eightConnected) {
    return std::make_unique<IFTAlgorithm>(eightConnected, false);
}

std::unique_ptr<IFTAlgorithm> createVerboseIFT(bool eightConnected) {
    return std::make_unique<IFTAlgorithm>(eightConnected, true);
}

std::unique_ptr<IFTResult> quickIFT(
    const Image& image,
    const PathCostFunction& costFunction,
    const SeedSet& seeds,
    bool eightConnected) {
    
    auto algorithm = createStandardIFT(eightConnected);
    return algorithm->runBasicIFT(image, costFunction, seeds);
}

// === MÉTODOS AUXILIARES INTERNOS ===

void IFTAlgorithm::initializeIFTMaps(
    IFTResult& result,
    const Image& image,
    const PathCostFunction& costFunction,
    const SeedSet& seeds) {
    
    // Esta função é chamada pelos algoritmos otimizados para inicializar
    // os mapas IFT antes do processamento principal
    result.initializeForProcessing(image, seeds);
    
    if (verbose) {
        std::cout << "Mapas IFT inicializados para imagem " 
                  << image.getWidth() << "x" << image.getHeight() 
                  << " com " << seeds.getActiveSeeds().size() << " sementes" << std::endl;
    }
} 