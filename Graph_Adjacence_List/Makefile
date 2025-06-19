# Diretórios
SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
OPENCV_DIR := third_party/opencv

# Arquivos fonte
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
TARGET := $(BUILD_DIR)/grafos

# Configuração do compilador
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 \
    -I$(INC_DIR) \
    -I$(OPENCV_DIR)/include

LDFLAGS := -L$(OPENCV_DIR)/x64/mingw/lib \
    -lopencv_core455 \
    -lopencv_imgproc455 \
    -lopencv_imgcodecs455 \
    -lopencv_highgui455

# Regra padrão
all: $(TARGET)

# Como gerar o binário
$(TARGET): $(SRCS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Limpar a pasta de build
clean:
	rm -rf $(BUILD_DIR)

# Limpar e recompilar tudo
rebuild: clean all

# Compilar e executar
run: all
	./$(TARGET)
