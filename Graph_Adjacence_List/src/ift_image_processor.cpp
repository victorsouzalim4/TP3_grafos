#include <iostream>
#include <string>
#include <chrono>
#include <opencv2/opencv.hpp>

#include "image.h"
#include "seed_set.h"
#include "path_cost_function.h"
#include "ift_algorithm.h"
#include "ift_optimized_algorithm.h"
#include "utils/opencv_ift_bridge.h"

using namespace std;
using namespace std::chrono;

void showUsage() {
    cout << "\n=== PROCESSADOR DE IMAGENS IFT ===" << endl;
    cout << "Uso: ./ift_image_processor <imagem> [opções]" << endl;
    cout << "\nOpções:" << endl;
    cout << "  -i, --interactive    Seleção interativa de sementes" << endl;
    cout << "  -a, --automatic N    Gerar N sementes automáticas (padrão: 4)" << endl;
    cout << "  -f, --function FUNC  Função de custo: sum, max (padrão: sum)" << endl;
    cout << "  -w, --weight WEIGHT  Peso do arco: diff, grad, const (padrão: diff)" << endl;
    cout << "  -o, --output FILE    Arquivo de saída (padrão: resultado_ift.png)" << endl;
    cout << "  -s, --show           Exibir resultado na tela" << endl;
    cout << "  -opt, --optimized    Usar algoritmo otimizado" << endl;
    cout << "\nExemplos:" << endl;
    cout << "  ./ift_image_processor imagem1.png -i -s" << endl;
    cout << "  ./ift_image_processor imagem2.png -a 6 -f max -w grad -o segmentacao.png" << endl;
}

int main(int argc, char* argv[]) {
    try {
        cout << "🚀 INICIANDO PROCESSADOR IFT..." << endl;
        cout << "Argumentos recebidos: " << argc << endl;
        for (int i = 0; i < argc; i++) {
            cout << "  argv[" << i << "] = " << argv[i] << endl;
        }
    
    if (argc < 2) {
        showUsage();
        return 1;
    }
    
    string inputFile = argv[1];
    bool interactive = false;
    bool automatic = true;
    int numSeeds = 4;
    string costFunction = "sum";
    string weightType = "diff";
    string outputFile = "resultado_ift.png";
    bool showResults = false;
    bool useOptimized = false;
    
    // Parse argumentos
    for (int i = 2; i < argc; i++) {
        string arg = argv[i];
        
        if (arg == "-i" || arg == "--interactive") {
            interactive = true;
            automatic = false;
        }
        else if (arg == "-a" || arg == "--automatic") {
            if (i + 1 < argc) {
                numSeeds = atoi(argv[++i]);
                automatic = true;
                interactive = false;
                // Validação crítica
                if (numSeeds <= 0) {
                    cerr << "❌ Erro: Número de sementes deve ser maior que 0. Valor fornecido: " << argv[i] << endl;
                    return 1;
                }
            } else {
                cerr << "❌ Erro: Opção -a requer um número de sementes." << endl;
                return 1;
            }
        }
        else if (arg == "-f" || arg == "--function") {
            if (i + 1 < argc) {
                costFunction = argv[++i];
            }
        }
        else if (arg == "-w" || arg == "--weight") {
            if (i + 1 < argc) {
                weightType = argv[++i];
            }
        }
        else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            }
        }
        else if (arg == "-s" || arg == "--show") {
            showResults = true;
        }
        else if (arg == "-opt" || arg == "--optimized") {
            useOptimized = true;
        }
    }
    
    try {
        cout << "\n🖼️  PROCESSADOR IFT DE IMAGENS REAIS" << endl;
        cout << "=====================================" << endl;
        
        // Carrega imagem
        cout << "📁 Carregando imagem: " << inputFile << endl;
        cv::Mat originalMat = cv::imread(inputFile);
        if (originalMat.empty()) {
            cerr << "❌ Erro: Não foi possível carregar a imagem!" << endl;
            return 1;
        }
        
        cout << "📏 Dimensões: " << originalMat.cols << "x" << originalMat.rows 
             << " (" << originalMat.channels() << " canais)" << endl;
        
        // Converte para nossa classe Image
        Image image = OpenCVIFTBridge::cvMatToImageGray(originalMat);
        cout << "✅ Imagem convertida para escala de cinza" << endl;
        
        // Configura sementes
        SeedSet seedSet;
        
        if (interactive) {
            cout << "\n🎯 Modo interativo selecionado" << endl;
            cv::Mat displayMat;
            if (originalMat.channels() == 3) {
                displayMat = originalMat.clone();
            } else {
                cv::cvtColor(originalMat, displayMat, cv::COLOR_GRAY2BGR);
            }
            
            bool seedsSelected = OpenCVIFTBridge::selectSeedsInteractive(displayMat, seedSet, image);
            if (!seedsSelected) {
                cout << "❌ Operação cancelada pelo usuário." << endl;
                return 0;
            }
        }
        else if (automatic) {
            cout << "\n🤖 Gerando " << numSeeds << " sementes automáticas..." << endl;
            seedSet = OpenCVIFTBridge::generateAutomaticSeeds(image, numSeeds);
        }
        
        cout << "🌱 Total de sementes: " << seedSet.getActiveSeeds().size() << endl;
        
        // Configura função de custo
        cout << "\n⚙️  Configurando algoritmo..." << endl;
        cout << "   Função de custo: " << costFunction << endl;
        cout << "   Tipo de peso: " << weightType << endl;
        cout << "   Algoritmo: " << (useOptimized ? "Otimizado" : "Básico") << endl;
        
        unique_ptr<PathCostFunction> pathCost;
        
        if (costFunction == "max") {
            if (weightType == "grad") {
                pathCost = make_unique<ConfigurableMaxPathCost>(make_unique<GradientWeight>());
            } else if (weightType == "const") {
                pathCost = make_unique<ConfigurableMaxPathCost>(make_unique<ConstantWeight>(1));
            } else {
                pathCost = make_unique<ConfigurableMaxPathCost>(make_unique<IntensityDifferenceWeight>());
            }
        } else {
            if (weightType == "grad") {
                pathCost = make_unique<ConfigurableAdditivePathCost>(make_unique<GradientWeight>());
            } else if (weightType == "const") {
                pathCost = make_unique<ConfigurableAdditivePathCost>(make_unique<ConstantWeight>(1));
            } else {
                pathCost = make_unique<ConfigurableAdditivePathCost>(make_unique<IntensityDifferenceWeight>());
            }
        }
        
        // Executa IFT
        cout << "\n🚀 Executando IFT..." << endl;
        auto startTime = high_resolution_clock::now();
        
        std::unique_ptr<IFTResult> resultPtr;
        if (useOptimized) {
            OptimizedIFTAlgorithm algorithm;
            resultPtr = algorithm.runOptimizedIFT(image, *pathCost, seedSet);
        } else {
            IFTAlgorithm algorithm;
            resultPtr = algorithm.runBasicIFT(image, *pathCost, seedSet);
        }
        
        if (!resultPtr) {
            cerr << "❌ Erro: Falha na execução do algoritmo IFT!" << endl;
            return 1;
        }
        
        IFTResult& result = *resultPtr;
        
        auto endTime = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(endTime - startTime);
        
        cout << "✅ Processamento concluído!" << endl;
        cout << "⏱️  Tempo: " << duration.count() << " ms" << endl;
        
        // Analisa resultado
        map<int, int> labelCount;
        for (int y = 0; y < image.getHeight(); y++) {
            for (int x = 0; x < image.getWidth(); x++) {
                Pixel pixel = image.getPixel(x, y);
                int label = result.getLabel(pixel);
                labelCount[label]++;
            }
        }
        
        cout << "\n📊 Distribuição de rótulos:" << endl;
        int totalPixels = image.getWidth() * image.getHeight();
        for (const auto& pair : labelCount) {
            double percentage = (100.0 * pair.second) / totalPixels;
            cout << "   Rótulo " << pair.first << ": " << pair.second 
                 << " pixels (" << fixed << setprecision(1) << percentage << "%)" << endl;
        }
        
        // Salva resultado
        cout << "\n💾 Salvando resultado..." << endl;
        bool saved = OpenCVIFTBridge::saveIFTResult(outputFile, image, result);
        if (saved) {
            cout << "✅ Resultado salvo em: " << outputFile << endl;
        } else {
            cerr << "❌ Erro ao salvar resultado!" << endl;
        }
        
        // Exibe resultado (se solicitado)
        if (showResults) {
            cout << "\n👁️  Exibindo resultados..." << endl;
            
            cv::Mat originalDisplay = originalMat.clone();
            cv::Mat resultVisualization = OpenCVIFTBridge::visualizeIFTResult(image, result);
            
            // Redimensiona se necessário para caber na tela
            int maxWidth = 800;
            int maxHeight = 600;
            
            if (originalDisplay.cols > maxWidth || originalDisplay.rows > maxHeight) {
                double scale = min(double(maxWidth) / originalDisplay.cols, 
                                 double(maxHeight) / originalDisplay.rows);
                cv::resize(originalDisplay, originalDisplay, cv::Size(0, 0), scale, scale);
                cv::resize(resultVisualization, resultVisualization, cv::Size(0, 0), scale, scale);
            }
            
            cv::namedWindow("Imagem Original", cv::WINDOW_AUTOSIZE);
            cv::namedWindow("Resultado IFT", cv::WINDOW_AUTOSIZE);
            
            cv::imshow("Imagem Original", originalDisplay);
            cv::imshow("Resultado IFT", resultVisualization);
            
            cout << "Pressione qualquer tecla para fechar..." << endl;
            cv::waitKey(0);
            cv::destroyAllWindows();
        }
        
        cout << "\n🎉 Processamento concluído com sucesso!" << endl;
        
    } catch (const exception& e) {
        cerr << "❌ Erro: " << e.what() << endl;
        return 1;
    }
    
    return 0;
    
    } catch (const exception& e) {
        cerr << "❌ Erro Fatal: " << e.what() << endl;
        cerr << "🔧 Sugestões:" << endl;
        cerr << "   1. Verifique se as DLLs do OpenCV estão no PATH" << endl;
        cerr << "   2. Verifique se o arquivo de imagem existe" << endl;
        cerr << "   3. Tente: set PATH=%PATH%;third_party\\opencv\\x64\\mingw\\bin" << endl;
        return 1;
    } catch (...) {
        cerr << "❌ Erro Fatal Desconhecido!" << endl;
        cerr << "🔧 Sugestões:" << endl;
        cerr << "   1. Verifique dependências do OpenCV" << endl;
        cerr << "   2. Recompile o projeto" << endl;
        return 1;
    }
} 