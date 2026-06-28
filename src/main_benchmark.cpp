#include "Benchmark.h"
#include <iostream>
#include <vector>
#include <string>

// ============================================================================
//  main_benchmark.cpp
//  Programa de CONSOLA (no usa Qt) que corre la comparacion experimental y
//  imprime la tabla de resultados. Es el ejecutable que se usa para llenar
//  el reporte. Se puede pasar tamanos y frames por linea de comandos:
//
//      ./quadtree_bench                  -> 1000 5000 10000, 15 frames
//      ./quadtree_bench 2000 8000 20000  -> esos tamanos, 15 frames
//      ./quadtree_bench --frames 30 1000 5000 10000
// ============================================================================

int main(int argc, char** argv) {
    std::vector<int> tamanos;
    int frames = 15;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--frames" && i + 1 < argc) {
            frames = std::stoi(argv[++i]);
        } else {
            try { tamanos.push_back(std::stoi(arg)); } catch (...) { /* ignorar */ }
        }
    }

    if (tamanos.empty()) {
        ejecutarBenchmark(std::cout);                  // tamanos por defecto
    } else {
        ejecutarBenchmark(std::cout, tamanos, frames); // tamanos del usuario
    }
    return 0;
}
