#include "Benchmark.h"
#include "Simulacion.h"

#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>

// ============================================================================
//  Benchmark.cpp
// ============================================================================

namespace {
    using Reloj = std::chrono::high_resolution_clock;

    const char* nombreDistribucion(Distribucion d) {
        switch (d) {
            case Distribucion::Uniforme:     return "UNIFORME";
            case Distribucion::Clusters:     return "CLUSTERS";
            case Distribucion::AltaDensidad: return "ALTA DENSIDAD";
        }
        return "?";
    }
}

void ejecutarBenchmark(std::ostream& out, std::vector<int> tamanos, int frames) {
    const Distribucion distribuciones[] = {
        Distribucion::Uniforme,
        Distribucion::Clusters,
        Distribucion::AltaDensidad
    };

    out << "\n================ COMPARACION EXPERIMENTAL: QuadTree vs Fuerza Bruta ================\n";
    out << "Frames promediados por caso: " << frames << "\n";
    out << "(Tiempos en milisegundos. 'cand/part' = candidatos revisados por particula con QuadTree)\n\n";

    for (Distribucion distrib : distribuciones) {
        out << "----- Distribucion: " << nombreDistribucion(distrib) << " -----\n";
        // Encabezado de la tabla.
        out << std::left  << std::setw(9)  << "n"
            << std::right << std::setw(12) << "build(ms)"
            << std::setw(12) << "FB(ms/f)"
            << std::setw(12) << "QT(ms/f)"
            << std::setw(14) << "comp.FB"
            << std::setw(14) << "comp.QT"
            << std::setw(11) << "cand/part"
            << std::setw(11) << "speedup" << "\n";

        for (int n : tamanos) {
            Simulacion sim;
            // 800x600 es solo el mundo del benchmark; los parametros restantes
            // son los tipicos del enunciado (capacidad 4, radio 3, velocidad 1).
            sim.configurar(n, 800.0, 600.0, /*capacidad*/4, /*radio*/3.0,
                           /*velocidadMax*/1.0, distrib);
            sim.generar();

            double sumaBuild = 0.0;   // tiempo de construccion del arbol
            double sumaFB    = 0.0;   // tiempo de fuerza bruta por frame
            double sumaQT    = 0.0;   // tiempo de quadtree por frame
            long long sumaCompFB = 0; // comparaciones de fuerza bruta
            long long sumaCompQT = 0; // comparaciones de quadtree

            for (int f = 0; f < frames; ++f) {
                sim.mover();

                // --- Tiempo de construccion del QuadTree ---
                const auto tb0 = Reloj::now();
                auto arbol = sim.construirArbol();
                sumaBuild += std::chrono::duration<double>(Reloj::now() - tb0).count();

                // --- Fuerza bruta (sin marcar colisiones: solo medimos) ---
                Metricas mFB = sim.detectarFuerzaBruta(false);
                sumaFB     += mFB.tiempoSegundos;
                sumaCompFB += mFB.comparaciones;

                // --- QuadTree ---
                Metricas mQT = sim.detectarConQuadTree(*arbol, false);
                sumaQT     += mQT.tiempoSegundos;
                sumaCompQT += mQT.comparaciones;
            }

            // Promedios por frame.
            const double buildMs = (sumaBuild / frames) * 1000.0;
            const double fbMs    = (sumaFB    / frames) * 1000.0;
            const double qtMs    = (sumaQT    / frames) * 1000.0;
            const double compFB  = static_cast<double>(sumaCompFB) / frames;
            const double compQT  = static_cast<double>(sumaCompQT) / frames;
            const double candPorParticula = compQT / n;
            const double speedup = (qtMs > 0.0) ? (fbMs / qtMs) : 0.0;

            out << std::left  << std::setw(9)  << n
                << std::right << std::fixed << std::setprecision(3)
                << std::setw(12) << buildMs
                << std::setw(12) << fbMs
                << std::setw(12) << qtMs
                << std::setprecision(0)
                << std::setw(14) << compFB
                << std::setw(14) << compQT
                << std::setprecision(2)
                << std::setw(11) << candPorParticula
                << std::setw(10) << speedup << "x" << "\n";
        }
        out << "\n";
    }

    out << "Interpretacion esperada: el QuadTree revisa muy pocos candidatos por\n"
           "particula, asi que sus comparaciones crecen aprox. de forma lineal con n,\n"
           "mientras que la fuerza bruta crece de forma cuadratica. El speedup aumenta\n"
           "con n. En 'alta densidad' el QuadTree pierde algo de ventaja porque muchas\n"
           "particulas caen en la misma region y los nodos se llenan mas.\n";
}
