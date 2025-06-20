#ifndef IMAGE_H 
#define IMAGE_H

#include <vector>
#include <string>
#include <stdexcept>
#include <pixel.h>

class Image {
    private:
        std::vector<std::vector<uint8_t>> data;
        int width, height;

    public:
        // Construtores
        Image(int width, int height, uint8_t defaultValue = 0);
        Image(const std::vector<std::vector<uint8_t>>& imageData);

        // Acesso básico
        int getWidth() const { return width; }
        int getHeight() const { return height; }

        // Acesso aos pixels
        uint8_t getPixelValue(int x, int y) const;
        void setPixelValue(int x, int y, uint8_t value);
        Pixel getPixel(int x, int y) const;
        
        // Aliases para compatibilidade
        uint8_t getPixelIntensity(int x, int y) const { return getPixelValue(x, y); }
        void setPixel(int x, int y, uint8_t value) { setPixelValue(x, y, value); }

        bool isValidCoordinate(int x, int y) const;

        // Para integração com sistema atual
        std::vector<std::vector<uint8_t>> getRawData() const { return data; }

        std::vector<Pixel> getAllPixels() const;
        std::vector<Pixel> getNeighbors(const Pixel& pixel, bool eightConnected = false) const;

       void print() const;
       bool saveToFile(const std::string& filename) const;
};

#endif