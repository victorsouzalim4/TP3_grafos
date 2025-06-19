#include "pixel.h"
#include <sstream>
#include <string>

// Operadores de comparação - essenciais para STL containers
bool Pixel::operator==(const Pixel& other) const {
    // Dois pixels são iguais se têm mesmas coordenadas E mesma intensidade
    return x == other.x && y == other.y && intensity == other.intensity;
}

bool Pixel::operator!=(const Pixel& other) const {
    return !(*this == other);
}

bool Pixel::operator<(const Pixel& other) const {
    // Ordenação lexicográfica: primeiro por y (linha), depois por x (coluna)
    // Convenção padrão em processamento de imagens (row-major order)
    if (y != other.y) {
        return y < other.y;
    }
    if (x != other.x) {
        return x < other.x;
    }
    // Se coordenadas são iguais, ordena por intensidade
    return intensity < other.intensity;
}

// Cálculo de distância euclidiana - importante para funções de custo do IFT
double Pixel::distanceTo(const Pixel& other) const {
    // Distância euclidiana no plano ℤ²
    // d(s,t) = √((x₁-x₂)² + (y₁-y₂)²)
    int dx = x - other.x;
    int dy = y - other.y;
    return std::sqrt(dx * dx + dy * dy);
}

// Conversão para índice linear - compatibilidade com sistema atual
int Pixel::toLinearIndex(int imageWidth) const {
    // Fórmula: index = y * width + x
    // Convenção row-major: percorre linha por linha
    return y * imageWidth + x;
}

// Conversão de índice linear para pixel - operação inversa
Pixel Pixel::fromLinearIndex(int linearIndex, int imageWidth) {
    // Operação inversa: y = index / width, x = index % width
    int pixelY = linearIndex / imageWidth;
    int pixelX = linearIndex % imageWidth;
    return Pixel(pixelX, pixelY, 0);  // intensidade será definida depois
}

// String para debug e visualização
std::string Pixel::toString() const {
    std::ostringstream oss;
    oss << "Pixel(" << x << "," << y << "," << static_cast<int>(intensity) << ")";
    return oss.str();
}

// Hash function para unordered containers - essencial para performance
std::size_t PixelHash::operator()(const Pixel& p) const {
    // Combina hash das coordenadas de forma eficiente
    // Usa números primos para evitar colisões
    std::size_t h1 = std::hash<int>{}(p.x);
    std::size_t h2 = std::hash<int>{}(p.y);
    std::size_t h3 = std::hash<uint8_t>{}(p.intensity);
    
    // Combine hashes usando XOR e shift
    return h1 ^ (h2 << 1) ^ (h3 << 2);
} 