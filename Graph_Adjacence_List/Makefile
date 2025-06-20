# Makefile CORRIGIDO - Remove linkagem estática incompatível com .dll.a
# Diretórios
SRC_DIR := src
INC_DIR := include
OPENCV_DIR := third_party/opencv
BUILD_DIR := build

# Arquivos fonte
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
TARGET := $(BUILD_DIR)/ift_image_processor.exe

# Arquivos fonte específicos do IFT
IFT_SOURCES := $(SRC_DIR)/ift/data_structures/pixel.cpp \
               $(SRC_DIR)/ift/data_structures/image.cpp \
               $(SRC_DIR)/ift/data_structures/seed_set.cpp \
               $(SRC_DIR)/ift/algorithms/path_cost_function.cpp \
               $(SRC_DIR)/ift/ift_result.cpp \
               $(SRC_DIR)/ift/algorithms/ift_algorithm.cpp \
               $(SRC_DIR)/ift/data_structures/bucket_queue.cpp \
               $(SRC_DIR)/ift/algorithms/ift_optimized_algorithm.cpp \
               $(SRC_DIR)/utils/opencv_ift_bridge.cpp \
               $(SRC_DIR)/ift_image_processor.cpp

# Target principal


# Configuração do compilador SEM linkagem estática conflitante
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O2 -g \
    -I$(INC_DIR) \
    -I$(OPENCV_DIR)/include

# Linkagem compatível com .dll.a (sem flags estáticas)
LDFLAGS := -L$(OPENCV_DIR)/x64/mingw/lib \
    -lopencv_core455 \
    -lopencv_imgproc455 \
    -lopencv_imgcodecs455 \
    -lopencv_highgui455 \
    -v

# Regra principal
all: $(TARGET)

# Como gerar o binário
$(TARGET): $(IFT_SOURCES)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Limpar
clean:
	@if exist $(BUILD_DIR)\*.exe del $(BUILD_DIR)\*.exe
	@if exist $(BUILD_DIR)\*.o del $(BUILD_DIR)\*.o

run: all
	./$(TARGET)

# Teste com PATH correto
test: $(TARGET)
	@echo "Testando versão corrigida..."
	@set PATH=%PATH%;$(OPENCV_DIR)\x64\mingw\bin && .\$(TARGET)

.PHONY: all clean test 