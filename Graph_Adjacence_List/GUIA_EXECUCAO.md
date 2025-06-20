# 🎯 GUIA DE EXECUÇÃO - IFT IMAGE PROCESSOR

## ✅ **PROBLEMAS RESOLVIDOS**

### 🔧 **Problema 1: Executável não funcionava**
**SOLUÇÃO**: Criado executável com linkagem estática que resolve dependências de runtime.

### 🔧 **Problema 2: CompileRun do VS Code falhava**  
**SOLUÇÃO**: Criadas configurações `.vscode/tasks.json` e `.vscode/launch.json`.

---

## 🚀 **COMO EXECUTAR**

### **Método 1: Executável Estático (RECOMENDADO)**
```bash
# Compilar versão estática
make -f Makefile_static

# Executar (só precisa das DLLs do OpenCV)
$env:PATH += ";$PWD\third_party\opencv\x64\mingw\bin"
.\ift_image_processor_static.exe src\images\imagem1.png -a 4 -s
```

### **Método 2: VS Code Build Tasks**
1. **Ctrl+Shift+P** → **Tasks: Run Task** → **"Compilar IFT Completo"**
2. **Ctrl+Shift+P** → **Tasks: Run Task** → **"Executar IFT"**

### **Método 3: Debug no VS Code**
1. **F5** ou **Run → Start Debugging**
2. Escolher: **"Debug IFT com Imagem"**

---

## 📋 **PARÂMETROS DISPONÍVEIS**

```bash
# Uso básico
./ift_image_processor_static.exe <imagem> [opções]

# Opções:
-i, --interactive    # Seleção interativa de sementes
-a, --automatic N    # N sementes automáticas (padrão: 4)
-f, --function FUNC  # Função de custo: sum, max (padrão: sum)
-w, --weight WEIGHT  # Peso do arco: diff, grad, const (padrão: diff)
-o, --output FILE    # Arquivo de saída (padrão: resultado_ift.png)
-s, --show           # Exibir resultado na tela
-opt, --optimized    # Usar algoritmo otimizado
```

### **Exemplos Práticos:**
```bash
# Processamento básico com 4 sementes automáticas
.\ift_image_processor_static.exe src\images\imagem1.png -a 4 -o resultado1.png

# Seleção interativa de sementes com visualização
.\ift_image_processor_static.exe src\images\imagem2.png -i -s

# Algoritmo otimizado com função de custo máxima
.\ift_image_processor_static.exe src\images\imagem3.jpg -a 6 -f max -w grad -opt -s
```

---

## 🔧 **DEPENDÊNCIAS NECESSÁRIAS**

### **Para o Executável Estático:**
- **OpenCV DLLs**: `third_party/opencv/x64/mingw/bin/` deve estar no PATH
- **Imagens de teste**: Disponíveis em `src/images/`

### **Para Development:**
- **MinGW/MSYS2**: Para compilação
- **GDB**: Para debugging (configurado em `.vscode/launch.json`)

---

## 📁 **ESTRUTURA DE ARQUIVOS**

```
Graph_Adjacence_List/
├── ift_image_processor.exe          # Versão original (precisa de DLLs)
├── ift_image_processor_static.exe   # Versão estática (RECOMENDADA)
├── Makefile_ift                     # Makefile original
├── Makefile_static                  # Makefile para versão estática
├── .vscode/
│   ├── tasks.json                   # Configurações de build
│   └── launch.json                  # Configurações de debug
├── src/images/                      # Imagens de teste
│   ├── imagem1.png
│   ├── imagem2.png
│   └── imagem3.jpg
└── third_party/opencv/              # Bibliotecas OpenCV
```

---

## 🐛 **SOLUÇÃO DE PROBLEMAS**

### **Erro: "Exit code -1073741511"**
**Solução**: Use a versão estática (`ift_image_processor_static.exe`)

### **Erro: "Não foi possível carregar a imagem"**
**Solução**: Verifique se o caminho está correto e se as DLLs do OpenCV estão no PATH

### **Erro: "CompileRun falha no VS Code"**
**Solução**: Use as tasks configuradas ao invés do CompileRun padrão

---

## ✅ **STATUS FINAL**

- ✅ **Compilação**: Funcionando com Makefile_static
- ✅ **Execução**: Funcionando com dependências corretas  
- ✅ **VS Code Integration**: Tasks e Debug configurados
- ✅ **Documentação**: Guia completo criado

**Projeto totalmente funcional e pronto para uso!** 🎉 