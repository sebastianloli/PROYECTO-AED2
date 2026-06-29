#pragma once

#include <vector>
#include <iosfwd>

// tamanos: cantidades de particulas a probar (por defecto 1.000 / 5.000 / 10.000).
// frames:  cuantos frames se promedian por caso.
void ejecutarBenchmark(std::ostream &out,
                       std::vector<int> tamanos = {1000, 5000, 10000},
                       int frames = 15);
