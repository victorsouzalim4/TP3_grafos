#include "image.h"
#include <iostream>
#include <iomanip>
#include <fstream>

// Construtor com dimensões - cria imagem em branco ou com valor padrão
Image::Image(int width, int height, uint8_t defaultValue) 
    : width(width), height(height) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Image dimensions must be positive");
    }
    
    // Inicializa matriz 2D com valor padrão
    data.resize(height);
    for (int y = 0; y < height; ++y) {
        data[y].resize(width, defaultValue);
    }
}

// Construtor a partir de dados existentes
Image::Image(const std::vector<std::vector<uint8_t>>& imageData) {
    if (imageData.empty() || imageData[0].empty()) {
        throw std::invalid_argument("Image data cannot be empty");
    }
    
    height = imageData.size();
    width = imageData[0].size();
    
    // Verifica se todas as linhas têm o mesmo tamanho
    for (const auto& row : imageData) {
        if (static_cast<int>(row.size()) != width) {
            throw std::invalid_argument("All rows must have the same width");
        }
    }
    
    data = imageData;
}

// Acesso seguro ao valor do pixel
uint8_t Image::getPixelValue(int x, int y) const {
    if (!isValidCoordinate(x, y)) {
        throw std::out_of_range("Pixel coordinates out of image bounds");
    }
    return data[y][x];  // Nota: data[linha][coluna] = data[y][x]
}

// Modificação segura do valor do pixel
void Image::setPixelValue(int x, int y, uint8_t value) {
    if (!isValidCoordinate(x, y)) {
        throw std::out_of_range("Pixel coordinates out of image bounds");
    }
    data[y][x] = value;
}

// Retorna pixel completo (coordenadas + intensidade)
Pixel Image::getPixel(int x, int y) const {
    if (!isValidCoordinate(x, y)) {
        throw std::out_of_range("Pixel coordinates out of image bounds");
    }
    return Pixel(x, y, data[y][x]);
}

// Validação de coordenadas - essencial para evitar segfaults
bool Image::isValidCoordinate(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

// Retorna todos os pixels da imagem - útil para inicialização do IFT
std::vector<Pixel> Image::getAllPixels() const {
    std::vector<Pixel> pixels;
    pixels.reserve(width * height);  // Otimização: pre-aloca memória
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            pixels.emplace_back(x, y, data[y][x]);
        }
    }
    
    return pixels;
}

// Calcula vizinhos válidos - implementa relação de adjacência A
std::vector<Pixel> Image::getNeighbors(const Pixel& pixel, bool eightConnected) const {
    std::vector<Pixel> neighbors;
    
    // Definição das direções de adjacência baseada no artigo IFT
    std::vector<std::pair<int, int>> directions;
    
    if (eightConnected) {
        // 8-conectividade: inclui diagonais
        // Corresponde a ε = √2 no artigo (Euclidean adjacency)
        directions = {
            {-1, -1}, {-1,  0}, {-1,  1},  // linha superior
            { 0, -1},           { 0,  1},  // linha atual (sem centro)
            { 1, -1}, { 1,  0}, { 1,  1}   // linha inferior
        };
    } else {
        // 4-conectividade: apenas adjacentes diretos
        // Corresponde a ε = 1 no artigo (4-connected adjacency)
        directions = {
            {-1,  0},  // cima
            { 0, -1},  // esquerda
            { 0,  1},  // direita
            { 1,  0}   // baixo
        };
    }
    
    // Calcula coordenadas dos vizinhos e verifica validade
    for (const auto& dir : directions) {
        int newX = pixel.x + dir.second;  // dx
        int newY = pixel.y + dir.first;   // dy
        
        if (isValidCoordinate(newX, newY)) {
            neighbors.emplace_back(newX, newY, data[newY][newX]);
        }
    }
    
    return neighbors;
}

// Visualização da imagem - útil para debug
void Image::print() const {
    std::cout << "Image " << width << "x" << height << ":\n";
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            std::cout << std::setw(3) << static_cast<int>(data[y][x]) << " ";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

// Salvamento simples em formato de texto (opcional)
bool Image::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Salva no formato PGM simples (P2)
    file << "P2\n";
    file << width << " " << height << "\n";
    file << "255\n";
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            file << static_cast<int>(data[y][x]);
            if (x < width - 1) file << " ";
        }
        file << "\n";
    }
    
    return file.good();
} 