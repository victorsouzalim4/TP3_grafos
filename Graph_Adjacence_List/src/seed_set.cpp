#include "seed_set.h"
#include "image.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <limits>

// === IMPLEMENTAÇÃO DA STRUCT SEED ===

std::string Seed::toString() const {
    std::ostringstream oss;
    oss << "Seed{" << pixel.toString() 
        << ", label=" << label 
        << ", handicap=" << handicap 
        << ", active=" << (active ? "true" : "false");
    if (!name.empty()) {
        oss << ", name='" << name << "'";
    }
    oss << "}";
    return oss.str();
}

// === IMPLEMENTAÇÃO DA CLASSE SEEDSET ===

// Adiciona semente com parâmetros específicos
void SeedSet::addSeed(const Pixel& pixel, int label, double handicap, const std::string& name) {
    // Se label não foi especificado (-1), usa próximo automático
    int actualLabel = (label == -1) ? nextLabel++ : label;
    
    // Verifica se já existe semente neste pixel
    auto it = pixelToIndex.find(pixel);
    if (it != pixelToIndex.end()) {
        // Atualiza semente existente
        Seed& existingSeed = seeds[it->second];
        existingSeed.label = actualLabel;
        existingSeed.handicap = handicap;
        existingSeed.active = true;
        if (!name.empty()) {
            existingSeed.name = name;
        }
    } else {
        // Cria nova semente
        Seed newSeed(pixel, actualLabel, handicap, true, name);
        
        // Adiciona às estruturas de dados
        int index = seeds.size();
        seeds.push_back(newSeed);
        seedPixels.insert(pixel);
        pixelToIndex[pixel] = index;
    }
}

// Adiciona semente simples com coordenadas
void SeedSet::addSeed(int x, int y, uint8_t intensity) {
    Pixel pixel(x, y, intensity);
    addSeed(pixel);
}

// Remove semente específica
bool SeedSet::removeSeed(const Pixel& pixel) {
    auto it = pixelToIndex.find(pixel);
    if (it == pixelToIndex.end()) {
        return false;  // Semente não encontrada
    }
    
    int index = it->second;
    
    // Remove das estruturas de dados
    seedPixels.erase(pixel);
    pixelToIndex.erase(it);
    
    // Remove do vetor (swap com último elemento para eficiência)
    if (index < static_cast<int>(seeds.size()) - 1) {
        // Atualiza índice do elemento que foi movido
        Pixel movedPixel = seeds.back().pixel;
        seeds[index] = seeds.back();
        pixelToIndex[movedPixel] = index;
    }
    seeds.pop_back();
    
    return true;
}

// Remove todas as sementes
void SeedSet::clear() {
    seeds.clear();
    seedPixels.clear();
    pixelToIndex.clear();
    nextLabel = 1;
}

// Ativa/desativa semente
bool SeedSet::setSeedActive(const Pixel& pixel, bool active) {
    auto it = pixelToIndex.find(pixel);
    if (it == pixelToIndex.end()) {
        return false;
    }
    
    seeds[it->second].active = active;
    return true;
}

// === CONSULTAS ===

// Verifica se pixel é semente ativa
bool SeedSet::isSeed(const Pixel& pixel) const {
    auto it = pixelToIndex.find(pixel);
    if (it == pixelToIndex.end()) {
        return false;
    }
    return seeds[it->second].active;
}

// Verifica se pixel tem semente (incluindo inativas)
bool SeedSet::hasSeed(const Pixel& pixel) const {
    return pixelToIndex.find(pixel) != pixelToIndex.end();
}

// Obtém label da semente
int SeedSet::getSeedLabel(const Pixel& pixel) const {
    auto it = pixelToIndex.find(pixel);
    if (it == pixelToIndex.end()) {
        return -1;  // Não é semente
    }
    return seeds[it->second].label;
}

// Obtém handicap da semente  
double SeedSet::getSeedHandicap(const Pixel& pixel) const {
    auto it = pixelToIndex.find(pixel);
    if (it == pixelToIndex.end()) {
        return std::numeric_limits<double>::infinity();  // Custo infinito se não é semente
    }
    return seeds[it->second].handicap;
}

// Obtém ponteiro para semente (ou nullptr se não existe)
const Seed* SeedSet::getSeed(const Pixel& pixel) const {
    auto it = pixelToIndex.find(pixel);
    if (it == pixelToIndex.end()) {
        return nullptr;
    }
    return &seeds[it->second];
}

// === ACESSO A COLEÇÕES ===

// Retorna apenas sementes ativas
std::vector<Seed> SeedSet::getActiveSeeds() const {
    std::vector<Seed> activeSeeds;
    for (const auto& seed : seeds) {
        if (seed.active) {
            activeSeeds.push_back(seed);
        }
    }
    return activeSeeds;
}

// Retorna pixels das sementes ativas
std::vector<Pixel> SeedSet::getActiveSeedPixels() const {
    std::vector<Pixel> activePixels;
    for (const auto& seed : seeds) {
        if (seed.active) {
            activePixels.push_back(seed.pixel);
        }
    }
    return activePixels;
}

// Retorna sementes por label
std::vector<Seed> SeedSet::getSeedsByLabel(int label) const {
    std::vector<Seed> labelSeeds;
    for (const auto& seed : seeds) {
        if (seed.label == label && seed.active) {
            labelSeeds.push_back(seed);
        }
    }
    return labelSeeds;
}

// === ESTATÍSTICAS ===

// Conta sementes ativas
size_t SeedSet::activeCount() const {
    size_t count = 0;
    for (const auto& seed : seeds) {
        if (seed.active) {
            count++;
        }
    }
    return count;
}

// Lista labels ativos
std::vector<int> SeedSet::getActiveLabels() const {
    std::vector<int> labels;
    for (const auto& seed : seeds) {
        if (seed.active) {
            // Adiciona label se ainda não estiver na lista
            if (std::find(labels.begin(), labels.end(), seed.label) == labels.end()) {
                labels.push_back(seed.label);
            }
        }
    }
    std::sort(labels.begin(), labels.end());
    return labels;
}

// === VALIDAÇÃO ===

// Verifica se sementes estão dentro dos limites da imagem
bool SeedSet::validateSeeds(const Image& image) const {
    for (const auto& seed : seeds) {
        if (seed.active && !image.isValidCoordinate(seed.pixel.x, seed.pixel.y)) {
            return false;
        }
    }
    return true;
}

// === DEBUG E VISUALIZAÇÃO ===

void SeedSet::print() const {
    std::cout << "SeedSet: " << seeds.size() << " total, " << activeCount() << " active" << std::endl;
    
    for (size_t i = 0; i < seeds.size(); ++i) {
        std::cout << "  [" << i << "] " << seeds[i].toString() << std::endl;
    }
    
    std::vector<int> labels = getActiveLabels();
    std::cout << "Active labels: ";
    for (int label : labels) {
        std::cout << label << " ";
    }
    std::cout << std::endl;
}

std::string SeedSet::toString() const {
    std::ostringstream oss;
    oss << "SeedSet{" << seeds.size() << " total, " << activeCount() << " active, labels=[";
    
    std::vector<int> labels = getActiveLabels();
    for (size_t i = 0; i < labels.size(); ++i) {
        if (i > 0) oss << ",";
        oss << labels[i];
    }
    oss << "]}";
    return oss.str();
}

// === CONFIGURAÇÕES ESPECIAIS ===

// Define handicaps baseados na intensidade dos pixels
void SeedSet::setHandicapsFromIntensity() {
    for (auto& seed : seeds) {
        if (seed.active) {
            seed.handicap = static_cast<double>(seed.pixel.intensity);
        }
    }
}

// Define handicaps uniformes
void SeedSet::setUniformHandicaps(double handicap) {
    for (auto& seed : seeds) {
        if (seed.active) {
            seed.handicap = handicap;
        }
    }
}

// Adiciona pixels da borda como sementes (útil para alguns algoritmos)
void SeedSet::addBorderSeeds(const Image& image, int label, double handicap) {
    int width = image.getWidth();
    int height = image.getHeight();
    
    // Borda superior e inferior
    for (int x = 0; x < width; ++x) {
        // Borda superior (y = 0)
        Pixel topPixel = image.getPixel(x, 0);
        addSeed(topPixel, label, handicap, "border_top");
        
        // Borda inferior (y = height-1)
        Pixel bottomPixel = image.getPixel(x, height - 1);
        addSeed(bottomPixel, label, handicap, "border_bottom");
    }
    
    // Borda esquerda e direita (excluindo cantos já adicionados)
    for (int y = 1; y < height - 1; ++y) {
        // Borda esquerda (x = 0)
        Pixel leftPixel = image.getPixel(0, y);
        addSeed(leftPixel, label, handicap, "border_left");
        
        // Borda direita (x = width-1)
        Pixel rightPixel = image.getPixel(width - 1, y);
        addSeed(rightPixel, label, handicap, "border_right");
    }
} 