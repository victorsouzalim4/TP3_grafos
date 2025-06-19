#ifndef PATH_COST_FUNCTION_H
#define PATH_COST_FUNCTION_H

#include <vector>
#include <limits>
#include <cmath>
#include <memory>
#include "pixel.h"
#include "image.h"
#include "seed_set.h"

// Representa um caminho como sequência de pixels
typedef std::vector<Pixel> Path;

// Interface base para funções de custo de caminho conforme artigo IFT
class PathCostFunction {
public:
    virtual ~PathCostFunction() = default;
    
    // === MÉTODOS FUNDAMENTAIS DO ARTIGO IFT ===
    
    // Calcula h(t) - handicap/custo inicial para pixel semente
    virtual double getHandicap(const Pixel& pixel, const SeedSet& seeds) const;
    
    // Calcula w(s,t) - peso do arco entre pixels adjacentes  
    virtual double getArcWeight(const Pixel& from, const Pixel& to, const Image& image) const = 0;
    
    // Computa f(π·⟨s,t⟩) dado f(π) e w(s,t) - operação incremental
    virtual double extendCost(double currentPathCost, double arcWeight) const = 0;
    
    // === MÉTODOS AUXILIARES ===
    
    // Calcula custo de caminho completo f(π) - para validação
    virtual double computePathCost(const Path& path, const Image& image, const SeedSet& seeds) const;
    
    // Verifica se função satisfaz condições de monotonicidade (MI)
    virtual bool isMonotonicIncremental() const = 0;
    
    // Nome da função para debug
    virtual std::string getName() const = 0;
    
protected:
    // Constante para custo infinito (+∞)
    static constexpr double INFINITY_COST = std::numeric_limits<double>::infinity();
};

// === IMPLEMENTAÇÕES ESPECÍFICAS ===

// Função de custo aditiva: f_sum(π·⟨s,t⟩) = f_sum(π) + w(s,t)
class AdditivePathCost : public PathCostFunction {
public:
    double extendCost(double currentPathCost, double arcWeight) const override {
        // Se custo atual é infinito, mantém infinito
        if (currentPathCost == INFINITY_COST) {
            return INFINITY_COST;
        }
        return currentPathCost + arcWeight;
    }
    
    bool isMonotonicIncremental() const override { return true; }
    std::string getName() const override { return "f_sum (Additive)"; }
};

// Função de custo máximo: f_max(π·⟨s,t⟩) = max{f_max(π), w(s,t)}
class MaxPathCost : public PathCostFunction {
public:
    double extendCost(double currentPathCost, double arcWeight) const override {
        // Se custo atual é infinito, mantém infinito
        if (currentPathCost == INFINITY_COST) {
            return INFINITY_COST;
        }
        return std::max(currentPathCost, arcWeight);
    }
    
    bool isMonotonicIncremental() const override { return true; }
    std::string getName() const override { return "f_max (Maximum)"; }
};

// === ESTRATÉGIAS PARA PESOS DE ARCO w(s,t) ===

// Interface para calcular pesos de arco
class ArcWeightStrategy {
public:
    virtual ~ArcWeightStrategy() = default;
    virtual double computeWeight(const Pixel& from, const Pixel& to, const Image& image) const = 0;
    virtual std::string getName() const = 0;
};

// Peso baseado em diferença de intensidade: |I(s) - I(t)|
class IntensityDifferenceWeight : public ArcWeightStrategy {
public:
    double computeWeight(const Pixel& from, const Pixel& to, const Image& image) const override {
        return std::abs(static_cast<double>(from.intensity) - static_cast<double>(to.intensity));
    }
    
    std::string getName() const override { return "Intensity Difference"; }
};

// Peso baseado no gradiente local (útil para detecção de bordas)
class GradientWeight : public ArcWeightStrategy {
private:
    double sigma;  // Parâmetro de suavização
    
public:
    GradientWeight(double sigma = 1.0) : sigma(sigma) {}
    
    double computeWeight(const Pixel& from, const Pixel& to, const Image& image) const override {
        // Gradiente local no ponto médio entre from e to
        double intensityDiff = std::abs(static_cast<double>(from.intensity) - static_cast<double>(to.intensity));
        return intensityDiff / (1.0 + sigma);
    }
    
    std::string getName() const override { return "Gradient Weight"; }
};

// Peso constante (útil para testes)
class ConstantWeight : public ArcWeightStrategy {
private:
    double weight;
    
public:
    ConstantWeight(double w = 1.0) : weight(w) {}
    
    double computeWeight(const Pixel& from, const Pixel& to, const Image& image) const override {
        return weight;
    }
    
    std::string getName() const override { return "Constant Weight"; }
};

// Peso baseado na intensidade do pixel destino (útil para watershed)
class DestinationIntensityWeight : public ArcWeightStrategy {
public:
    double computeWeight(const Pixel& from, const Pixel& to, const Image& image) const override {
        return static_cast<double>(to.intensity);
    }
    
    std::string getName() const override { return "Destination Intensity"; }
};

// === FUNÇÕES DE CUSTO CONFIGURÁVEIS ===

// Função de custo aditiva configurável com estratégia de peso
class ConfigurableAdditivePathCost : public AdditivePathCost {
private:
    std::unique_ptr<ArcWeightStrategy> weightStrategy;
    
public:
    ConfigurableAdditivePathCost(std::unique_ptr<ArcWeightStrategy> strategy) 
        : weightStrategy(std::move(strategy)) {}
    
    double getArcWeight(const Pixel& from, const Pixel& to, const Image& image) const override {
        return weightStrategy->computeWeight(from, to, image);
    }
    
    std::string getName() const override { 
        return "f_sum (" + weightStrategy->getName() + ")"; 
    }
};

// Função de custo máximo configurável com estratégia de peso
class ConfigurableMaxPathCost : public MaxPathCost {
private:
    std::unique_ptr<ArcWeightStrategy> weightStrategy;
    
public:
    ConfigurableMaxPathCost(std::unique_ptr<ArcWeightStrategy> strategy) 
        : weightStrategy(std::move(strategy)) {}
    
    double getArcWeight(const Pixel& from, const Pixel& to, const Image& image) const override {
        return weightStrategy->computeWeight(from, to, image);
    }
    
    std::string getName() const override { 
        return "f_max (" + weightStrategy->getName() + ")"; 
    }
};

// === DECLARAÇÕES DE FUNÇÕES AUXILIARES ===

// Funções de debug e teste
void printCostFunctionInfo(const PathCostFunction& costFunc);
void testCostFunction(const PathCostFunction& costFunc, const Path& path, 
                     const Image& image, const SeedSet& seeds);

// Factory functions para criar funções comuns
std::unique_ptr<PathCostFunction> createIntensityDifferenceSum();
std::unique_ptr<PathCostFunction> createIntensityDifferenceMax();
std::unique_ptr<PathCostFunction> createWatershedSum();
std::unique_ptr<PathCostFunction> createWatershedMax();
std::unique_ptr<PathCostFunction> createConstantSum(double weight = 1.0);
std::unique_ptr<PathCostFunction> createConstantMax(double weight = 1.0);

#endif 