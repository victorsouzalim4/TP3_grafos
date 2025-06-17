# Graph Adjacency List - Projeto em C++

Este projeto implementa grafos utilizando lista de adjacência em C++, com suporte a grafos direcionados e não direcionados.

---

## 🚀 Como rodar o projeto no Windows com VSCode + MSYS2

Este guia cobre **todos os passos necessários** para compilar e executar o projeto em um ambiente Windows, com terminal integrado no VSCode, utilizando MSYS2, g++ e Makefile.

---

## ✅ Requisitos

- Visual Studio Code
- Extensão C/C++ (Microsoft)
- MSYS2 (terminal e compilador)
- Projeto com a seguinte estrutura:

```
Graph_Adjacence_List/
├── src/             → Arquivos-fonte (.cpp)
├── include/         → Arquivos de cabeçalho (.h)
├── Makefile         → Arquivo de build
└── build/           → Pasta gerada automaticamente
```

---

## 🧩 Etapa 1: Instalar o MSYS2

1. Acesse: [https://www.msys2.org](https://www.msys2.org)
2. Baixe o instalador e execute normalmente
3. Instale na pasta padrão (`C:\msys64`)
4. Após instalar, abra o menu Iniciar e execute:  
   **MSYS2 MINGW64**

---

## 📦 Etapa 2: Instalar `make` e `g++` no MSYS2

No terminal **MSYS2 MINGW64**, digite os comandos:

```bash
pacman -Syu                      # Atualiza o sistema
pacman -S make                   # Instala o make
pacman -S mingw-w64-x86_64-gcc  # Instala o compilador g++
```

> ⚠️ Caso o terminal feche após `pacman -Syu`, reabra e continue.

---

## 🧪 Etapa 3: Testar se make e g++ funcionam

No mesmo terminal:

```bash
make --version
g++ --version
```

Se os dois comandos mostrarem versões corretamente, está tudo pronto para configurar o VSCode.

---

## 🛠️ Etapa 4: Configurar o terminal MSYS2 no VSCode

### 1. Abra o VSCode  
### 2. Pressione `Ctrl + Shift + P` e selecione:  
   **Preferences: Open Settings (JSON)**  
### 3. Cole o seguinte bloco no arquivo `settings.json`:

```json
"terminal.integrated.profiles.windows": {
  "MSYS2 MinGW64 Shell": {
    "path": "C:\\msys64\\usr\\bin\\bash.exe",
    "args": ["--login", "-i"],
    "env": {
      "MSYSTEM": "MINGW64",
      "CHERE_INVOKING": "1"
    },
    "icon": "terminal-bash"
  }
},
"terminal.integrated.defaultProfile.windows": "MSYS2 MinGW64 Shell"
```

> 🧠 Esse bloco define o terminal MSYS2 como padrão, usando o shell Bash com o ambiente `MINGW64`.

---

## 🖥️ Etapa 5: Abrir o terminal corretamente

1. No VSCode, vá até o menu:  
   `Terminal > New Terminal`

2. Ele abrirá automaticamente com a configuração do MSYS2 integrada (você verá algo como `MINGW64 ~`)

---

## 📁 Etapa 6: Navegar até a pasta do projeto

Suponha que seu projeto esteja em:

```
C:\Users\SeuNome\Documentos\Projetos\Graph_Adjacence_List
```

No terminal MSYS2 (dentro do VSCode), digite:

```bash
cd /c/Users/SeuNome/Documentos/Projetos/Graph_Adjacence_List
```

### Verifique se está no lugar certo:

```bash
ls
```

Você deve ver: `Makefile`, `src/`, `include/`, etc.

---

## ⚙️ Etapa 7: Compilar e executar o projeto

### 🧱 Compilar o projeto:

```bash
make
```

### ▶️ Compilar e rodar:

```bash
make run
```

### ♻️ Recompilar tudo do zero:

```bash
make rebuild
```

### 🧹 Limpar o diretório `build/`:

```bash
make clean
```

---

## 🧠 Informações adicionais

- O programa compilado ficará em:  
  `build/grafos`

- O comando `make` **só recompila arquivos modificados**, o que acelera builds.

- O terminal **PowerShell ou cmd.exe não funcionam com `*.cpp`** — por isso usamos o MSYS2 + Bash.

- O VSCode detecta automaticamente erros e exibe no editor se o `C_Cpp` estiver instalado.

---

## 🧪 Exemplos de comandos

```bash
cd /c/Users/Valle/OneDrive/Documentos/Grafos/Graphs/Graph_Adjacence_List
make run
```

---

## 📌 Dicas finais

- Sempre mantenha o `Makefile` na raiz do projeto (mesmo nível que `src/` e `include/`)
- Não copie a pasta `build/` entre máquinas — ela é gerada automaticamente
- Use `make clean` antes de mudanças importantes para evitar erros ocultos

---


## 🧪 Etapa Extra: Configurar e executar testes com Google Test

### 0. Instalar o CMake via MSYS2 (caso ainda não tenha)

No terminal MSYS2 MinGW64, execute:

```bash
pacman -Syu             # Atualiza o sistema
pacman -S cmake         # Instala o CMake
```

> Se o terminal fechar após o primeiro comando, reabra e continue.

---

### 1. Certifique-se de que o `Main.cpp` **não esteja incluso na lib**:

```cmake
file(GLOB SRC_FILES src/*.cpp)
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp)
add_library(graphlib ${SRC_FILES})
```

### 2. Certifique-se de que o CMake registre os testes corretamente:

```cmake
enable_testing()
include(GoogleTest)
gtest_discover_tests(tests)
add_test(NAME RunTests COMMAND tests)  # fallback manual
```

### 3. Comandos para compilar e rodar testes:

```bash
cmake -S . -B build -G "Unix Makefiles"
make -C build
./build/tests             # Executa diretamente
cd build && ctest --output-on-failure  # Via CTest
```

---

## 🧼 Dica: reconstruir tudo do zero

```bash
rm -rf build
cmake -S . -B build -G "Unix Makefiles"
make -C build
```

---

## 👨‍💻 Autor

Projeto desenvolvido por Victor Souza Lima.

Se este README te ajudou, compartilhe ou mantenha no repositório do projeto como referência 🚀
