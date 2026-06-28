#include "Benchmark.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char **argv)
{
    std::vector<int> tamanos;
    int frames = 15;

    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];
        if (arg == "--frames" && i + 1 < argc)
        {
            frames = std::stoi(argv[++i]);
        }
        else
        {
            try
            {
                tamanos.push_back(std::stoi(arg));
            }
            catch (...)
            { /* ignorar */
            }
        }
    }

    if (tamanos.empty())
    {
        ejecutarBenchmark(std::cout); // tamanos por defecto
    }
    else
    {
        ejecutarBenchmark(std::cout, tamanos, frames); // tamanos del usuario
    }
    return 0;
}
