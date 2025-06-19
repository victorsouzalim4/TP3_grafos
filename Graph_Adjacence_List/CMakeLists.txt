cmake_minimum_required(VERSION 3.14)
project(Graph_Adjacence_List CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Inclui headers
include_directories(include)

# Caminhos CORRETOS para OpenCV com MinGW
set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/third_party/opencv")
set(OpenCV_INCLUDE_DIR "${OpenCV_DIR}/include")
set(OpenCV_LIB_DIR "${OpenCV_DIR}/x64/mingw/lib")
set(OpenCV_BIN_DIR "${OpenCV_DIR}/x64/mingw/bin")

# Incluir headers do OpenCV
include_directories(${OpenCV_INCLUDE_DIR})

# IMPORTANTE: link_directories deve vir ANTES das definições de executáveis
link_directories(${OpenCV_LIB_DIR})

# Bibliotecas OpenCV CORRETAS para MinGW (sem extensão .dll)
set(OpenCV_LIBS
    opencv_core455
    opencv_imgproc455
    opencv_highgui455
    opencv_imgcodecs455
)

# Coletar todos os arquivos fonte incluindo o novo arquivo OpenCV bridge
file(GLOB SRC_FILES 
    src/*.cpp
    src/*/*.cpp
    src/*/*/*.cpp
)
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/ift_image_processor.cpp)

# Lib principal
add_library(graphlib ${SRC_FILES})


# Executável do processador de imagens IFT
add_executable(IFTImageProcessor src/ift_image_processor.cpp)
target_link_libraries(IFTImageProcessor graphlib ${OpenCV_LIBS})

# Adicionar o diretório bin do OpenCV ao PATH durante o build (para encontrar DLLs)
if(WIN32)
    set_target_properties(IFTImageProcessor PROPERTIES
        VS_DEBUGGER_ENVIRONMENT "PATH=${OpenCV_BIN_DIR};$ENV{PATH}"
    )
endif()

# Comentando testes por enquanto - focar no IFT principal
# GoogleTest e testes foram temporariamente desabilitados
