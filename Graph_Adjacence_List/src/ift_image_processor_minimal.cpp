#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "=== TESTE MINIMAL ===" << std::endl;
    std::cout << "Program started successfully!" << std::endl;
    std::cout << "argc = " << argc << std::endl;
    
    for (int i = 0; i < argc; i++) {
        std::cout << "argv[" << i << "] = " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    
    std::cout << "Test completed successfully!" << std::endl;
    return 0;
} 