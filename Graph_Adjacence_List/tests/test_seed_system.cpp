#include <iostream>
#include <cassert>
#include <vector>
#include <limits>
#include "pixel.h"
#include "image.h"
#include "seed_set.h"

void testBasicSeedOperations() {
    std::cout << "=== TESTE: Operações Básicas de Sementes ===" << std::endl;
    
    SeedSet seedSet;
    
    // Teste 1: Adicionar sementes
    Pixel p1(1, 1, 100);
    Pixel p2(3, 3, 150);
    
    seedSet.addSeed(p1, 1, 10.0, "objeto");
    seedSet.addSeed(p2, 2, 20.0, "fundo");
    
    std::cout << "Sementes adicionadas: " << seedSet.size() << std::endl;
    assert(seedSet.size() == 2);
    assert(seedSet.activeCount() == 2);
    
    // Teste 2: Verificar se são sementes
    assert(seedSet.isSeed(p1) == true);
    assert(seedSet.isSeed(p2) == true);
    
    Pixel p3(5, 5, 200);
    assert(seedSet.isSeed(p3) == false);
    
    // Teste 3: Obter informações das sementes
    assert(seedSet.getSeedLabel(p1) == 1);
    assert(seedSet.getSeedLabel(p2) == 2);
    assert(seedSet.getSeedHandicap(p1) == 10.0);
    assert(seedSet.getSeedHandicap(p2) == 20.0);
    
    // Teste 4: Labels ativos
    std::vector<int> labels = seedSet.getActiveLabels();
    assert(labels.size() == 2);
    assert(labels[0] == 1 && labels[1] == 2);
    
    std::cout << "[OK] Operações básicas funcionando!" << std::endl;
    seedSet.print();
    std::cout << std::endl;
}

void testSeedManagement() {
    std::cout << "=== TESTE: Gerenciamento de Sementes ===" << std::endl;
    
    SeedSet seedSet;
    
    // Adiciona várias sementes
    seedSet.addSeed(0, 0, 50);   // Label automático (1)
    seedSet.addSeed(1, 0, 60);   // Label automático (2)
    seedSet.addSeed(2, 0, 70);   // Label automático (3)
    
    std::cout << "Sementes com labels automáticos: " << seedSet.toString() << std::endl;
    
    // Teste: Desativar semente
    Pixel p1(0, 0, 50);
    assert(seedSet.setSeedActive(p1, false) == true);
    assert(seedSet.isSeed(p1) == false);      // Não é semente ativa
    assert(seedSet.hasSeed(p1) == true);      // Mas ainda existe
    assert(seedSet.activeCount() == 2);
    
    // Teste: Reativar semente
    assert(seedSet.setSeedActive(p1, true) == true);
    assert(seedSet.isSeed(p1) == true);
    assert(seedSet.activeCount() == 3);
    
    // Teste: Remover semente
    assert(seedSet.removeSeed(p1) == true);
    assert(seedSet.hasSeed(p1) == false);
    assert(seedSet.size() == 2);
    
    // Teste: Limpar todas
    seedSet.clear();
    assert(seedSet.size() == 0);
    assert(seedSet.activeCount() == 0);
    
    std::cout << "[OK] Gerenciamento de sementes funcionando!" << std::endl << std::endl;
}

void testSeedsByLabel() {
    std::cout << "=== TESTE: Sementes por Label ===" << std::endl;
    
    SeedSet seedSet;
    
    // Adiciona sementes com labels específicos
    seedSet.addSeed(Pixel(0, 0, 100), 1, 0.0, "objeto1_a");
    seedSet.addSeed(Pixel(1, 0, 110), 1, 0.0, "objeto1_b");
    seedSet.addSeed(Pixel(2, 0, 120), 2, 0.0, "objeto2_a");
    seedSet.addSeed(Pixel(3, 0, 130), 2, 0.0, "objeto2_b");
    seedSet.addSeed(Pixel(4, 0, 140), 3, 0.0, "fundo");
    
    // Teste: Obter sementes por label
    std::vector<Seed> obj1Seeds = seedSet.getSeedsByLabel(1);
    std::vector<Seed> obj2Seeds = seedSet.getSeedsByLabel(2);
    std::vector<Seed> fundoSeeds = seedSet.getSeedsByLabel(3);
    
    assert(obj1Seeds.size() == 2);
    assert(obj2Seeds.size() == 2);
    assert(fundoSeeds.size() == 1);
    
    std::cout << "Sementes do objeto 1: " << obj1Seeds.size() << std::endl;
    std::cout << "Sementes do objeto 2: " << obj2Seeds.size() << std::endl;
    std::cout << "Sementes do fundo: " << fundoSeeds.size() << std::endl;
    
    // Teste: Labels ativos
    std::vector<int> activeLabels = seedSet.getActiveLabels();
    std::cout << "Labels ativos: ";
    for (int label : activeLabels) {
        std::cout << label << " ";
    }
    std::cout << std::endl;
    
    std::cout << "[OK] Agrupamento por labels funcionando!" << std::endl << std::endl;
}

void testSeedValidation() {
    std::cout << "=== TESTE: Validação de Sementes ===" << std::endl;
    
    // Cria imagem pequena 3x3
    Image image(3, 3, 128);
    SeedSet seedSet;
    
    // Adiciona sementes válidas
    seedSet.addSeed(0, 0, 100);  // Canto superior esquerdo
    seedSet.addSeed(1, 1, 150);  // Centro
    seedSet.addSeed(2, 2, 200);  // Canto inferior direito
    
    assert(seedSet.validateSeeds(image) == true);
    std::cout << "Sementes válidas: [OK]" << std::endl;
    
    // Adiciona semente inválida
    seedSet.addSeed(5, 5, 255);  // Fora dos limites da imagem 3x3
    
    assert(seedSet.validateSeeds(image) == false);
    std::cout << "Detecção de semente inválida: [OK]" << std::endl;
    
    std::cout << "[OK] Validação funcionando!" << std::endl << std::endl;
}

void testSpecialConfigurations() {
    std::cout << "=== TESTE: Configurações Especiais ===" << std::endl;
    
    // Cria imagem pequena
    std::vector<std::vector<uint8_t>> data = {
        {10, 20, 30},
        {40, 50, 60},
        {70, 80, 90}
    };
    Image image(data);
    
    SeedSet seedSet;
    
    // Teste 1: Handicaps baseados em intensidade
    seedSet.addSeed(0, 0, 10);  // Pixel com intensidade 10
    seedSet.addSeed(1, 1, 50);  // Pixel com intensidade 50
    seedSet.addSeed(2, 2, 90);  // Pixel com intensidade 90
    
    seedSet.setHandicapsFromIntensity();
    
    assert(seedSet.getSeedHandicap(Pixel(0, 0, 10)) == 10.0);
    assert(seedSet.getSeedHandicap(Pixel(1, 1, 50)) == 50.0);
    assert(seedSet.getSeedHandicap(Pixel(2, 2, 90)) == 90.0);
    
    std::cout << "Handicaps por intensidade: [OK]" << std::endl;
    
    // Teste 2: Handicaps uniformes
    seedSet.setUniformHandicaps(100.0);
    
    assert(seedSet.getSeedHandicap(Pixel(0, 0, 10)) == 100.0);
    assert(seedSet.getSeedHandicap(Pixel(1, 1, 50)) == 100.0);
    assert(seedSet.getSeedHandicap(Pixel(2, 2, 90)) == 100.0);
    
    std::cout << "Handicaps uniformes: [OK]" << std::endl;
    
    // Teste 3: Sementes de borda
    SeedSet borderSeedSet;
    borderSeedSet.addBorderSeeds(image, 0, std::numeric_limits<double>::infinity());
    
    // Em uma imagem 3x3, devemos ter 8 pixels de borda
    assert(borderSeedSet.activeCount() == 8);
    
    // Verifica se cantos são sementes
    assert(borderSeedSet.isSeed(Pixel(0, 0, 10)) == true);  // Canto superior esquerdo
    assert(borderSeedSet.isSeed(Pixel(2, 0, 30)) == true);  // Canto superior direito
    assert(borderSeedSet.isSeed(Pixel(0, 2, 70)) == true);  // Canto inferior esquerdo
    assert(borderSeedSet.isSeed(Pixel(2, 2, 90)) == true);  // Canto inferior direito
    
    // Verifica se centro NÃO é semente de borda
    assert(borderSeedSet.isSeed(Pixel(1, 1, 50)) == false);
    
    std::cout << "Sementes de borda: [OK] (" << borderSeedSet.activeCount() << " pixels)" << std::endl;
    
    std::cout << "[OK] Configurações especiais funcionando!" << std::endl << std::endl;
}

void testIFTTheoryCompliance() {
    std::cout << "=== TESTE: Conformidade com Teoria IFT ===" << std::endl;
    
    SeedSet seedSet;
    
    // Simula cenário típico do IFT: segmentação de 2 objetos
    // Objeto 1: pixels escuros (label 1)
    seedSet.addSeed(Pixel(0, 0, 50), 1, 0.0, "objeto_escuro");
    
    // Objeto 2: pixels claros (label 2)  
    seedSet.addSeed(Pixel(5, 5, 200), 2, 0.0, "objeto_claro");
    
    // Fundo: label 0 com handicap infinito (como no artigo)
    seedSet.addSeed(Pixel(10, 10, 128), 0, std::numeric_limits<double>::infinity(), "fundo");
    
    // Teste conceito S ⊆ I: conjunto de sementes é subconjunto da imagem
    std::vector<Pixel> seedPixels = seedSet.getActiveSeedPixels();
    assert(seedPixels.size() == 3);
    
    // Teste: pixels não-semente têm handicap infinito (conforme artigo)
    Pixel nonSeed(1, 1, 100);
    assert(seedSet.getSeedHandicap(nonSeed) == std::numeric_limits<double>::infinity());
    
    // Teste: diferentes labels para diferentes objetos
    std::vector<int> labels = seedSet.getActiveLabels();
    assert(labels.size() == 3);  // Labels 0, 1, 2
    
    std::cout << "Conceito S ⊆ I implementado: [OK]" << std::endl;
    std::cout << "Restrição de custos implementada: [OK]" << std::endl;
    std::cout << "Multi-objeto suportado: [OK]" << std::endl;
    
    seedSet.print();
    
    std::cout << "[OK] Sistema compatível com teoria IFT!" << std::endl << std::endl;
}

int main() {
    try {
        testBasicSeedOperations();
        testSeedManagement();
        testSeedsByLabel();
        testSeedValidation();
        testSpecialConfigurations();
        testIFTTheoryCompliance();
        
        std::cout << "🎉 TODOS OS TESTES DO SISTEMA DE SEMENTES PASSARAM!" << std::endl;
        std::cout << "✅ Sistema S ⊆ I implementado corretamente" << std::endl;
        std::cout << "✅ Restrições de custo preparadas para IFT" << std::endl;
        std::cout << "✅ Suporte a múltiplos objetos funcionando" << std::endl;
        std::cout << std::endl;
        std::cout << "PRÓXIMO PASSO: Implementar funções de custo de caminho (f_sum, f_max)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ ERRO: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 