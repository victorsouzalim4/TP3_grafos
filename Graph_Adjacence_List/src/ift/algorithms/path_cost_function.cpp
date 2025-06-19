#include "path_cost_function.h"
#include <iostream>
#include <memory>

// === IMPLEMENTAÇÃO DA CLASSE BASE PathCostFunction ===

// Calcula h(t) - handicap do pixel conforme artigo IFT
double PathCostFunction::getHandicap(const Pixel& pixel, const SeedSet& seeds) const {
    // Conforme artigo: "setting h(t) = +∞ for pixels t ∉ S"
    if (!seeds.isSeed(pixel)) {
        return INFINITY_COST;  // Pixel não é semente -> custo infinito
    }
    
    // Se é semente, retorna handicap configurado
    return seeds.getSeedHandicap(pixel);
}

// Calcula custo de caminho completo f(π) - implementação de referência
double PathCostFunction::computePathCost(const Path& path, const Image& image, const SeedSet& seeds) const {
    if (path.empty()) {
        return INFINITY_COST;  // Caminho vazio é inválido
    }
    
    // Custo inicial: handicap do primeiro pixel
    double cost = getHandicap(path[0], seeds);
    
    // Se primeiro pixel não é semente válida, caminho é inválido
    if (cost == INFINITY_COST) {
        return INFINITY_COST;
    }
    
    // Acumula custos ao longo do caminho: f(π·⟨s,t⟩) = operação(f(π), w(s,t))
    for (size_t i = 1; i < path.size(); ++i) {
        const Pixel& from = path[i-1];
        const Pixel& to = path[i];
        
        // Calcula peso do arco w(s,t)
        double arcWeight = getArcWeight(from, to, image);
        
        // Aplica operação específica da função (soma ou máximo)
        cost = extendCost(cost, arcWeight);
        
        // Se custo se torna infinito, interrompe
        if (cost == INFINITY_COST) {
            break;
        }
    }
    
    return cost;
}

// === FUNÇÕES AUXILIARES PARA DEBUGGING ===

// Imprime informações detalhadas sobre uma função de custo
void printCostFunctionInfo(const PathCostFunction& costFunc) {
    std::cout << "=== Função de Custo ===" << std::endl;
    std::cout << "Nome: " << costFunc.getName() << std::endl;
    std::cout << "Monotônica-Incremental: " << (costFunc.isMonotonicIncremental() ? "Sim" : "Não") << std::endl;
    std::cout << "========================" << std::endl;
}

// Testa função de custo com caminho simples
void testCostFunction(const PathCostFunction& costFunc, const Path& path, 
                     const Image& image, const SeedSet& seeds) {
    std::cout << "\n=== TESTE: " << costFunc.getName() << " ===" << std::endl;
    
    if (path.empty()) {
        std::cout << "Caminho vazio - custo: +∞" << std::endl;
        return;
    }
    
    double totalCost = costFunc.computePathCost(path, image, seeds);
    
    std::cout << "Caminho: ";
    for (size_t i = 0; i < path.size(); ++i) {
        if (i > 0) std::cout << " -> ";
        std::cout << path[i].toString();
    }
    std::cout << std::endl;
    
    std::cout << "Custo total: ";
    if (totalCost == std::numeric_limits<double>::infinity()) {
        std::cout << "+∞";
    } else {
        std::cout << totalCost;
    }
    std::cout << std::endl;
    
    // Mostra decomposição do custo
    if (path.size() > 1 && totalCost != std::numeric_limits<double>::infinity()) {
        std::cout << "Decomposição:" << std::endl;
        
        double cost = costFunc.getHandicap(path[0], seeds);
        std::cout << "  h(" << path[0].toString() << ") = " << cost << std::endl;
        
        for (size_t i = 1; i < path.size(); ++i) {
            double arcWeight = costFunc.getArcWeight(path[i-1], path[i], image);
            cost = costFunc.extendCost(cost, arcWeight);
            
            std::cout << "  w(" << path[i-1].toString() << "," << path[i].toString() << ") = " 
                      << arcWeight << " -> custo = " << cost << std::endl;
        }
    }
    
    std::cout << "========================================" << std::endl;
}

// === FACTORY FUNCTIONS PARA CRIAR FUNÇÕES COMUNS ===

// Cria função f_sum com diferença de intensidade (mais comum)
std::unique_ptr<PathCostFunction> createIntensityDifferenceSum() {
    return std::make_unique<ConfigurableAdditivePathCost>(
        std::make_unique<IntensityDifferenceWeight>()
    );
}

// Cria função f_max com diferença de intensidade  
std::unique_ptr<PathCostFunction> createIntensityDifferenceMax() {
    return std::make_unique<ConfigurableMaxPathCost>(
        std::make_unique<IntensityDifferenceWeight>()
    );
}

// Cria função f_sum para watershed (intensidade do destino)
std::unique_ptr<PathCostFunction> createWatershedSum() {
    return std::make_unique<ConfigurableAdditivePathCost>(
        std::make_unique<DestinationIntensityWeight>()
    );
}

// Cria função f_max para watershed (intensidade do destino)
std::unique_ptr<PathCostFunction> createWatershedMax() {
    return std::make_unique<ConfigurableMaxPathCost>(
        std::make_unique<DestinationIntensityWeight>()
    );
}

// Cria função f_sum com peso constante (para testes)
std::unique_ptr<PathCostFunction> createConstantSum(double weight) {
    return std::make_unique<ConfigurableAdditivePathCost>(
        std::make_unique<ConstantWeight>(weight)
    );
}

// Cria função f_max com peso constante (para testes)
std::unique_ptr<PathCostFunction> createConstantMax(double weight) {
    return std::make_unique<ConfigurableMaxPathCost>(
        std::make_unique<ConstantWeight>(weight)
    );
} 