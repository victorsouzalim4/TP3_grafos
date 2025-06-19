#include <iostream>
#include <iomanip>
#include <memory>
#include "ift_algorithm.h"
#include "path_cost_function.h"
#include "pixel.h"
#include "image.h"
#include "seed_set.h"
#include "ift_result.h"

// Cria imagem de teste 5x5 com gradiente
Image createTestImage() {
    Image img(5, 5);
    
    // Gradiente diagonal: (0,0)=0, (1,1)=50, (2,2)=100, (3,3)=150, (4,4)=200
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            uint8_t intensity = static_cast<uint8_t>((x + y) * 25);
            img.setPixelValue(x, y, intensity);
        }
    }
    
    return img;
}

// Cria sementes de teste
SeedSet createTestSeeds() {
    SeedSet seeds;
    
    // Semente no canto (0,0) com handicap 0
    seeds.addSeed(Pixel(0, 0, 0), 1, 0.0);
    
    // Semente no centro (2,2) com handicap 5  
    seeds.addSeed(Pixel(2, 2, 100), 2, 5.0);
    
    return seeds;
}

// TESTE 1: Algoritmo básico com função aditiva
void testBasicIFTAdditive() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "TESTE 1: ALGORITMO IFT BÁSICO - FUNÇÃO ADITIVA" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    Image img = createTestImage();
    SeedSet seeds = createTestSeeds();
    auto costFunc = createIntensityDifferenceSum();
    
    // Cria algoritmo com debug
    auto algorithm = createVerboseIFT(false); // 4-conectado
    
    // Executa IFT
    auto result = algorithm->runBasicIFT(img, *costFunc, seeds);
    
    // Verifica resultados
    std::cout << "\n=== VERIFICAÇÃO DOS RESULTADOS ===" << std::endl;
    
    // Testa pixels específicos
    std::vector<Pixel> testPixels = {
        Pixel(0, 0, 0),    // Semente 1
        Pixel(1, 1, 50),   // Vizinho diagonal
        Pixel(2, 2, 100),  // Semente 2
        Pixel(4, 4, 200),  // Canto oposto
        Pixel(1, 0, 25)    // Vizinho direto
    };
    
    for (const Pixel& pixel : testPixels) {
        double cost = result->getCost(pixel);
        int label = result->getLabel(pixel);
        auto path = result->getOptimalPath(pixel);
        
        std::cout << "Pixel " << pixel.toString() << ":" << std::endl;
        std::cout << "  Custo: " << (cost == std::numeric_limits<double>::infinity() ? std::string("+∞") : std::to_string(cost)) << std::endl;
        std::cout << "  Label: " << label << std::endl;
        std::cout << "  Caminho: ";
        for (size_t i = 0; i < path.size(); ++i) {
            if (i > 0) std::cout << " -> ";
            std::cout << path[i].toString();
        }
        std::cout << std::endl << std::endl;
    }
    
    // Validação
    bool isValid = algorithm->validateResult(*result, img, *costFunc, seeds);
    std::cout << "Validação do resultado: " << (isValid ? "PASSOU ✓" : "FALHOU ✗") << std::endl;
    
    // Estatísticas
    auto stats = algorithm->getLastExecutionStats();
    std::cout << "\nEstatísticas:" << std::endl;
    std::cout << "  Pixels processados: " << stats.pixelsProcessed << "/25" << std::endl;
    std::cout << "  Tempo: " << stats.executionTimeMs << " ms" << std::endl;
    std::cout << "  Custo médio: " << std::fixed << std::setprecision(2) << stats.averageCostPerPixel << std::endl;
}

// TESTE 2: Algoritmo com função máximo
void testBasicIFTMaximum() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "TESTE 2: ALGORITMO IFT BÁSICO - FUNÇÃO MÁXIMO" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    Image img = createTestImage();
    SeedSet seeds = createTestSeeds();
    auto costFunc = createIntensityDifferenceMax();
    
    auto algorithm = createStandardIFT(false);
    auto result = algorithm->runBasicIFT(img, *costFunc, seeds);
    
    std::cout << "\n=== COMPARAÇÃO f_sum vs f_max ===" << std::endl;
    
    // Compara com resultado aditivo
    auto costFuncSum = createIntensityDifferenceSum();
    auto resultSum = algorithm->runBasicIFT(img, *costFuncSum, seeds);
    
    // Pixels de teste
    std::vector<Pixel> comparePixels = {
        Pixel(1, 1, 50),
        Pixel(3, 3, 150),
        Pixel(4, 0, 100)
    };
    
    std::cout << std::setw(15) << "Pixel" 
              << std::setw(12) << "f_sum" 
              << std::setw(12) << "f_max" 
              << std::setw(15) << "Diferença" << std::endl;
    std::cout << std::string(54, '-') << std::endl;
    
    for (const Pixel& pixel : comparePixels) {
        double costSum = resultSum->getCost(pixel);
        double costMax = result->getCost(pixel);
        double diff = costSum - costMax;
        
        std::cout << std::setw(15) << pixel.toString()
                  << std::setw(12) << std::fixed << std::setprecision(1) << costSum
                  << std::setw(12) << std::fixed << std::setprecision(1) << costMax  
                  << std::setw(15) << std::fixed << std::setprecision(1) << diff << std::endl;
    }
    
    bool isValid = algorithm->validateResult(*result, img, *costFunc, seeds);
    std::cout << "\nValidação f_max: " << (isValid ? "PASSOU ✓" : "FALHOU ✗") << std::endl;
}

// TESTE 3: Conectividade 4 vs 8
void testConnectivity() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "TESTE 3: CONECTIVIDADE 4 vs 8" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    Image img = createTestImage();
    SeedSet seeds = createTestSeeds();
    auto costFunc = createIntensityDifferenceSum();
    
    // Teste 4-conectado
    auto algorithm4 = createStandardIFT(false);
    auto result4 = algorithm4->runBasicIFT(img, *costFunc, seeds);
    
    // Teste 8-conectado  
    auto algorithm8 = createStandardIFT(true);
    auto result8 = algorithm8->runBasicIFT(img, *costFunc, seeds);
    
    std::cout << "\n=== COMPARAÇÃO 4-conectado vs 8-conectado ===" << std::endl;
    
    // Pixels diagonais são mais afetados
    std::vector<Pixel> diagonalPixels = {
        Pixel(1, 1, 50),
        Pixel(3, 1, 100),
        Pixel(1, 3, 100),
        Pixel(3, 3, 150)
    };
    
    std::cout << std::setw(15) << "Pixel" 
              << std::setw(12) << "4-conectado" 
              << std::setw(12) << "8-conectado" 
              << std::setw(15) << "Melhoria" << std::endl;
    std::cout << std::string(54, '-') << std::endl;
    
    for (const Pixel& pixel : diagonalPixels) {
        double cost4 = result4->getCost(pixel);
        double cost8 = result8->getCost(pixel);
        double improvement = cost4 - cost8;
        
        std::cout << std::setw(15) << pixel.toString()
                  << std::setw(12) << std::fixed << std::setprecision(1) << cost4
                  << std::setw(12) << std::fixed << std::setprecision(1) << cost8
                  << std::setw(15) << std::fixed << std::setprecision(1) << improvement << std::endl;
    }
    
    // Estatísticas
    auto stats4 = algorithm4->getLastExecutionStats();
    auto stats8 = algorithm8->getLastExecutionStats();
    
    std::cout << "\nEstatísticas comparativas:" << std::endl;
    std::cout << "4-conectado: " << stats4.averageCostPerPixel << " (custo médio)" << std::endl;
    std::cout << "8-conectado: " << stats8.averageCostPerPixel << " (custo médio)" << std::endl;
    std::cout << "Melhoria: " << (stats4.averageCostPerPixel - stats8.averageCostPerPixel) << std::endl;
}

// TESTE 4: Early termination 
void testEarlyTermination() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "TESTE 4: EARLY TERMINATION" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    Image img = createTestImage();
    SeedSet seeds = createTestSeeds();
    auto costFunc = createIntensityDifferenceSum();
    
    Pixel target(4, 4, 200); // Canto oposto
    
    auto algorithm = createVerboseIFT(false);
    
    std::cout << "Executando IFT até target " << target.toString() << "..." << std::endl;
    auto result = algorithm->runIFTToTarget(img, *costFunc, seeds, target);
    
    std::cout << "\nResultado:" << std::endl;
    std::cout << "Custo até target: " << result->getCost(target) << std::endl;
    
    auto path = result->getOptimalPath(target);
    std::cout << "Caminho ótimo: ";
    for (size_t i = 0; i < path.size(); ++i) {
        if (i > 0) std::cout << " -> ";
        std::cout << path[i].toString();
    }
    std::cout << std::endl;
    
    auto stats = algorithm->getLastExecutionStats();
    std::cout << "Pixels processados: " << stats.pixelsProcessed << std::endl;
    std::cout << "Tempo: " << stats.executionTimeMs << " ms" << std::endl;
}

// TESTE 5: Visualização da floresta
void testForestVisualization() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "TESTE 5: VISUALIZAÇÃO DA FLORESTA" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    Image img = createTestImage();
    SeedSet seeds = createTestSeeds();
    auto costFunc = createConstantSum(1.0); // Peso constante para visualização clara
    
    auto algorithm = createStandardIFT(false);
    auto result = algorithm->runBasicIFT(img, *costFunc, seeds);
    
    std::cout << "\nMapa de custos:" << std::endl;
    for (int y = 0; y < img.getHeight(); ++y) {
        for (int x = 0; x < img.getWidth(); ++x) {
            Pixel pixel = img.getPixel(x, y);
            double cost = result->getCost(pixel);
            
            if (cost == std::numeric_limits<double>::infinity()) {
                std::cout << std::setw(6) << "∞";
            } else {
                std::cout << std::setw(6) << std::fixed << std::setprecision(1) << cost;
            }
        }
        std::cout << std::endl;
    }
    
    std::cout << "\nMapa de labels:" << std::endl;
    for (int y = 0; y < img.getHeight(); ++y) {
        for (int x = 0; x < img.getWidth(); ++x) {
            Pixel pixel = img.getPixel(x, y);
            int label = result->getLabel(pixel);
            std::cout << std::setw(3) << label;
        }
        std::cout << std::endl;
    }
    
    std::cout << "\nVisualizacao ASCII:" << std::endl;
    std::cout << visualizeForest(*result, img) << std::endl;
}

int main() {
    std::cout << "################################################" << std::endl;
    std::cout << "#        TESTES DO ALGORITMO IFT BÁSICO       #" << std::endl;
    std::cout << "#           Implementação do Algoritmo 1       #" << std::endl;
    std::cout << "################################################" << std::endl;
    
    try {
        testBasicIFTAdditive();
        testBasicIFTMaximum();
        testConnectivity();
        testEarlyTermination();
        testForestVisualization();
        
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "✅ TODOS OS TESTES DO ALGORITMO IFT CONCLUÍDOS!" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ ERRO durante os testes: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 