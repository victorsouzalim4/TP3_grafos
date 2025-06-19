#ifndef SEED_SET_H
#define SEED_SET_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <limits>
#include "pixel.h"

// Estrutura que representa uma semente individual
struct Seed {
    Pixel pixel;           // Localização (x, y) da semente
    int label;             // Rótulo/ID do objeto (para segmentação multi-classe)
    double handicap;       // Custo inicial h(t) conforme artigo IFT
    bool active;           // Se a semente está ativa no processamento
    std::string name;      // Nome descritivo (opcional, para debug)
    
    // Construtores
    Seed() : pixel(), label(0), handicap(0.0), active(true), name("") {}
    
    Seed(const Pixel& p, int lbl = 0, double h = 0.0, bool act = true, const std::string& n = "") 
        : pixel(p), label(lbl), handicap(h), active(act), name(n) {}
    
    // Para debug
    std::string toString() const;
};

// Classe que gerencia o conjunto S ⊆ I de sementes
class SeedSet {
private:
    std::vector<Seed> seeds;                                    // Lista de todas as sementes
    std::unordered_set<Pixel, PixelHash> seedPixels;          // Para busca O(1) se pixel é semente
    std::unordered_map<Pixel, int, PixelHash> pixelToIndex;   // Mapeamento pixel -> índice na lista
    int nextLabel;                                             // Próximo label automático disponível
    
public:
    // Construtores
    SeedSet() : nextLabel(1) {}
    
    // === GERENCIAMENTO DE SEMENTES ===
    
    // Adiciona semente com parâmetros específicos
    void addSeed(const Pixel& pixel, int label = -1, double handicap = 0.0, 
                 const std::string& name = "");
    
    // Adiciona semente simples (label automático)
    void addSeed(int x, int y, uint8_t intensity = 0);
    
    // Remove semente específica
    bool removeSeed(const Pixel& pixel);
    
    // Remove todas as sementes
    void clear();
    
    // Ativa/desativa semente sem removê-la
    bool setSeedActive(const Pixel& pixel, bool active);
    
    // === CONSULTAS ===
    
    // Verifica se pixel é semente (ativa)
    bool isSeed(const Pixel& pixel) const;
    
    // Verifica se pixel é semente (incluindo inativas)  
    bool hasSeed(const Pixel& pixel) const;
    
    // Obtém informações da semente
    int getSeedLabel(const Pixel& pixel) const;
    double getSeedHandicap(const Pixel& pixel) const;
    const Seed* getSeed(const Pixel& pixel) const;
    
    // === ACESSO A COLEÇÕES ===
    
    // Retorna todas as sementes (ativas e inativas)
    std::vector<Seed> getAllSeeds() const { return seeds; }
    
    // Retorna apenas sementes ativas
    std::vector<Seed> getActiveSeeds() const;
    
    // Retorna apenas os pixels das sementes ativas
    std::vector<Pixel> getActiveSeedPixels() const;
    
    // Retorna sementes por label
    std::vector<Seed> getSeedsByLabel(int label) const;
    
    // === ESTATÍSTICAS ===
    
    size_t size() const { return seeds.size(); }
    size_t activeCount() const;
    std::vector<int> getActiveLabels() const;
    
    // === VALIDAÇÃO ===
    
    // Verifica se sementes são válidas para uma imagem
    bool validateSeeds(const class Image& image) const;
    
    // === DEBUG E VISUALIZAÇÃO ===
    
    void print() const;
    std::string toString() const;
    
    // === CONFIGURAÇÕES ESPECIAIS ===
    
    // Define handicaps baseados em intensidade (útil para alguns algoritmos)
    void setHandicapsFromIntensity();
    
    // Define handicaps uniformes para todas as sementes ativas
    void setUniformHandicaps(double handicap);
    
    // Configuração para border pixels como sementes (útil em alguns casos)
    void addBorderSeeds(const class Image& image, int label = 0, double handicap = std::numeric_limits<double>::infinity());
};

#endif 