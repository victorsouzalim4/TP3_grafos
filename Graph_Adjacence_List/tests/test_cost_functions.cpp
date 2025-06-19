#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include "path_cost_function.h"
#include "pixel.h"
#include "image.h"
#include "seed_set.h"

// Função auxiliar para criar uma imagem de teste
Image createTestImage() {
    Image img(5, 5);
    
    // Padrão de intensidades para testar diferentes pesos
    // Cria um gradiente diagonal
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            uint8_t intensity = static_cast<uint8_t>((x + y) * 25);  // 0, 25, 50, 75, 100...
            img.setPixelValue(x, y, intensity);  // Corrigido: usar setPixelValue
        }
    }
    
    return img;
}

// Função auxiliar para criar conjunto de sementes de teste
SeedSet createTestSeeds() {
    SeedSet seeds;  // Corrigido: usar construtor padrão
    
    // Adiciona sementes com handicaps diferentes
    seeds.addSeed(Pixel(0, 0, 0), 1, 0.0);    // Semente 1, handicap 0
    seeds.addSeed(Pixel(4, 4, 200), 2, 10.0); // Semente 2, handicap 10
    seeds.addSeed(Pixel(2, 2, 100), 3, 5.0);  // Semente 3, handicap 5
    
    return seeds;
}

// Testa propriedades básicas das funções de custo
void testBasicProperties() {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "TESTE 1: PROPRIEDADES BÁSICAS DAS FUNÇÕES DE CUSTO" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    auto costSum = createIntensityDifferenceSum();
    auto costMax = createIntensityDifferenceMax();
    
    std::cout << "\n--- Função Aditiva ---" << std::endl;
    printCostFunctionInfo(*costSum);
    
    std::cout << "\n--- Função Máximo ---" << std::endl;
    printCostFunctionInfo(*costMax);
    
    // Testa extensão de custo
    std::cout << "\n--- Teste de Extensão de Custo ---" << std::endl;
    double baseCost = 10.0;
    double arcWeight = 3.0;
    
    double sumResult = costSum->extendCost(baseCost, arcWeight);
    double maxResult = costMax->extendCost(baseCost, arcWeight);
    
    std::cout << "Custo base: " << baseCost << ", Peso arco: " << arcWeight << std::endl;
    std::cout << "f_sum: " << baseCost << " + " << arcWeight << " = " << sumResult << std::endl;
    std::cout << "f_max: max(" << baseCost << ", " << arcWeight << ") = " << maxResult << std::endl;
    
    // Testa com custo infinito
    double infResult1 = costSum->extendCost(std::numeric_limits<double>::infinity(), arcWeight);
    double infResult2 = costMax->extendCost(std::numeric_limits<double>::infinity(), arcWeight);
    
    std::cout << "\nTeste com custo infinito:" << std::endl;
    std::cout << "f_sum(+∞, " << arcWeight << ") = " << (infResult1 == std::numeric_limits<double>::infinity() ? "+∞" : std::to_string(infResult1)) << std::endl;
    std::cout << "f_max(+∞, " << arcWeight << ") = " << (infResult2 == std::numeric_limits<double>::infinity() ? "+∞" : std::to_string(infResult2)) << std::endl;
}

// Testa diferentes estratégias de peso
void testWeightStrategies() {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "TESTE 2: ESTRATÉGIAS DE PESO DE ARCO w(s,t)" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    Image img = createTestImage();
    
    // Pixels de teste
    Pixel p1(0, 0, 0);    // Intensidade 0
    Pixel p2(1, 1, 50);   // Intensidade 50  
    Pixel p3(2, 2, 100);  // Intensidade 100
    
    // Testa diferentes estratégias
    std::vector<std::unique_ptr<ArcWeightStrategy>> strategies;
    strategies.push_back(std::make_unique<IntensityDifferenceWeight>());
    strategies.push_back(std::make_unique<GradientWeight>(1.0));
    strategies.push_back(std::make_unique<ConstantWeight>(2.5));
    strategies.push_back(std::make_unique<DestinationIntensityWeight>());
    
    for (auto& strategy : strategies) {
        std::cout << "\n--- " << strategy->getName() << " ---" << std::endl;
        
        double w12 = strategy->computeWeight(p1, p2, img);
        double w23 = strategy->computeWeight(p2, p3, img);
        double w13 = strategy->computeWeight(p1, p3, img);
        
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "w(" << p1.toString() << ", " << p2.toString() << ") = " << w12 << std::endl;
        std::cout << "w(" << p2.toString() << ", " << p3.toString() << ") = " << w23 << std::endl;
        std::cout << "w(" << p1.toString() << ", " << p3.toString() << ") = " << w13 << std::endl;
    }
}

// Testa cálculo de handicaps
void testHandicaps() {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "TESTE 3: CÁLCULO DE HANDICAPS h(t)" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    SeedSet seeds = createTestSeeds();
    auto costFunc = createIntensityDifferenceSum();
    
    // Pixels de teste
    std::vector<Pixel> testPixels = {
        Pixel(0, 0, 0),      // Semente 1
        Pixel(4, 4, 200),    // Semente 2  
        Pixel(2, 2, 100),    // Semente 3
        Pixel(1, 1, 50),     // Não é semente
        Pixel(3, 3, 150)     // Não é semente
    };
    
    std::cout << std::fixed << std::setprecision(1);
    for (const auto& pixel : testPixels) {
        double handicap = costFunc->getHandicap(pixel, seeds);
        
        std::cout << "h(" << pixel.toString() << ") = ";
        if (handicap == std::numeric_limits<double>::infinity()) {
            std::cout << "+∞";
        } else {
            std::cout << handicap;
        }
        
        if (seeds.isSeed(pixel)) {
            std::cout << " (semente, label=" << seeds.getSeedLabel(pixel) << ")";
        } else {
            std::cout << " (não é semente)";
        }
        std::cout << std::endl;
    }
}

// Testa cálculo de custo de caminhos completos
void testPathCosts() {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "TESTE 4: CÁLCULO DE CUSTO DE CAMINHOS f(π)" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    Image img = createTestImage();
    SeedSet seeds = createTestSeeds();
    
    // Cria funções de custo para testar
    auto costSum = createIntensityDifferenceSum();
    auto costMax = createIntensityDifferenceMax();
    auto costConstSum = createConstantSum(1.0);
    
    // Define caminhos de teste
    std::vector<Path> testPaths = {
        // Caminho 1: Semente (0,0) -> (1,1) -> (2,2)
        {Pixel(0, 0, 0), Pixel(1, 1, 50), Pixel(2, 2, 100)},
        
        // Caminho 2: Semente (2,2) -> (3,3) -> (4,4) 
        {Pixel(2, 2, 100), Pixel(3, 3, 150), Pixel(4, 4, 200)},
        
        // Caminho 3: Não-semente (1,1) -> (2,2) - deve dar +∞
        {Pixel(1, 1, 50), Pixel(2, 2, 100)},
        
        // Caminho 4: Vazio - deve dar +∞
        {}
    };
    
    std::vector<std::unique_ptr<PathCostFunction>*> costFunctions = {
        &costSum, &costMax, &costConstSum
    };
    
    for (auto* costFuncPtr : costFunctions) {
        auto& costFunc = **costFuncPtr;
        
        for (size_t i = 0; i < testPaths.size(); ++i) {
            testCostFunction(costFunc, testPaths[i], img, seeds);
        }
    }
}

// Testa propriedade de monotonicidade
void testMonotonicity() {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "TESTE 5: PROPRIEDADE DE MONOTONICIDADE" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    Image img = createTestImage();
    SeedSet seeds = createTestSeeds();
    auto costSum = createIntensityDifferenceSum();
    
    // Caminho crescente: (0,0) -> (1,1) -> (2,2) -> (3,3)
    Path path = {
        Pixel(0, 0, 0),      // Semente, h=0
        Pixel(1, 1, 50),     // |0-50| = 50
        Pixel(2, 2, 100),    // |50-100| = 50  
        Pixel(3, 3, 150)     // |100-150| = 50
    };
    
    std::cout << "Testando monotonicidade em caminho crescente:" << std::endl;
    std::cout << "Caminho: ";
    for (size_t i = 0; i < path.size(); ++i) {
        if (i > 0) std::cout << " -> ";
        std::cout << path[i].toString();
    }
    std::cout << std::endl;
    
    // Calcula custos parciais
    double cost = costSum->getHandicap(path[0], seeds);
    std::cout << "f(⟨" << path[0].toString() << "⟩) = " << cost << std::endl;
    
    for (size_t i = 1; i < path.size(); ++i) {
        double arcWeight = costSum->getArcWeight(path[i-1], path[i], img);
        double newCost = costSum->extendCost(cost, arcWeight);
        
        std::cout << "f(π·⟨" << path[i-1].toString() << "," << path[i].toString() << "⟩) = "
                  << cost << " + " << arcWeight << " = " << newCost;
        
        if (newCost >= cost) {
            std::cout << " ✓ (monotônico)";
        } else {
            std::cout << " ✗ (não-monotônico)";
        }
        std::cout << std::endl;
        
        cost = newCost;
    }
}

// Função principal de teste
int main() {
    std::cout << "###############################################" << std::endl;
    std::cout << "#     TESTES DAS FUNÇÕES DE CUSTO IFT       #" << std::endl;
    std::cout << "###############################################" << std::endl;
    
    try {
        testBasicProperties();
        testWeightStrategies();
        testHandicaps();
        testPathCosts();
        testMonotonicity();
        
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "TODOS OS TESTES CONCLUÍDOS COM SUCESSO!" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\nERRO durante os testes: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 