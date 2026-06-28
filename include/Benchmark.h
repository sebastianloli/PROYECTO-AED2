#pragma once

#include <vector>
#include <iosfwd>

// ============================================================================
//  Benchmark.h
//  Comparacion experimental obligatoria: QuadTree vs Fuerza Bruta.
//  Recorre varios tamanos de entrada y las 3 distribuciones, y reporta
//  para cada caso: tiempo de construccion del arbol, tiempo promedio por
//  frame de cada metodo, comparaciones promedio, candidatos por particula
//  y el speedup. Imprime una tabla en el flujo de salida indicado.
// ============================================================================

// tamanos: cantidades de particulas a probar (por defecto 1.000 / 5.000 / 10.000).
// frames:  cuantos frames se promedian por caso.
void ejecutarBenchmark(std::ostream& out,
                       std::vector<int> tamanos = {1000, 5000, 10000},
                       int frames = 15);
