#pragma once

#include <vector>
#include <memory>
#include <random>
#include "Particula.h"
#include "QuadTree.h"

// ============================================================================
//  Simulacion.h
//  Logica del simulador, independiente de Qt (nucleo puro).
//  Mantiene la lista de particulas y ofrece:
//    - generacion de los 3 escenarios de distribucion,
//    - movimiento (un solo metodo, con rebote en bordes),
//    - construccion del QuadTree,
//    - deteccion de colisiones por fuerza bruta y por QuadTree,
//    - todos los parametros configurables que pide el enunciado.
// ============================================================================

// Escenarios de distribucion inicial requeridos por el enunciado.
enum class Distribucion {
    Uniforme    = 0,  // particulas repartidas de forma pareja
    Clusters    = 1,  // grupos concentrados en varios centros
    AltaDensidad = 2  // gran mayoria amontonada en el centro
};

// Resultado de una pasada de deteccion (sirve para los overlays y el reporte).
struct Metricas {
    long long comparaciones = 0;    // pares de particulas comparados
    double    tiempoSegundos = 0.0; // tiempo que tomo la deteccion
};

class Simulacion {
public:
    Simulacion();

    // ---- Parametros configurables (requisito del enunciado) ----
    void configurar(int n, double ancho, double alto,
                    int capacidadNodo, double radio, double velocidadMax,
                    Distribucion distrib);

    void establecerCantidad(int n)            { n_ = n; }
    void establecerCapacidad(int c)           { capacidadNodo_ = c; }
    void establecerRadio(double r)            { radio_ = r; }
    void establecerVelocidadMax(double v)     { velocidadMax_ = v; }
    void establecerDistribucion(Distribucion d){ distribucion_ = d; }

    // Ajusta el tamano del mundo (p. ej. al redimensionar la ventana Qt)
    // y reubica dentro a las particulas que hayan quedado fuera.
    void redimensionar(double ancho, double alto);

    // (Re)genera las particulas segun la distribucion y parametros actuales.
    void generar();

    // ---- Paso de simulacion ----
    // Mueve todas las particulas un frame y rebota en los bordes.
    void mover();

    // Construye un QuadTree nuevo con las particulas actuales.
    // Se reconstruye cada frame porque las particulas se mueven.
    std::unique_ptr<QuadTree> construirArbol();

    // Pone 'colisionando = false' en todas (se llama antes de cada deteccion).
    void resetColisiones();

    // SOLUCION INGENUA: compara todos contra todos -> O(n^2).
    // Si 'registrar' es true, marca 'colisionando' en las que chocan.
    Metricas detectarFuerzaBruta(bool registrar);

    // SOLUCION CON QUADTREE: para cada particula consulta solo su vecindad.
    Metricas detectarConQuadTree(const QuadTree& arbol, bool registrar);

    // ---- Accesores ----
    std::vector<Particula>&       particulas()       { return particulas_; }
    const std::vector<Particula>& particulas() const { return particulas_; }
    double ancho() const { return ancho_; }
    double alto()  const { return alto_;  }
    int    cantidad() const { return n_; }
    int    capacidadNodo() const { return capacidadNodo_; }
    double radio() const { return radio_; }

private:
    std::vector<Particula> particulas_;

    // Parametros configurables con valores por defecto razonables.
    int          n_            = 1000;
    double       ancho_        = 800.0;
    double       alto_         = 600.0;
    int          capacidadNodo_= 4;
    double       radio_        = 3.0;
    double       velocidadMax_ = 1.0;
    Distribucion distribucion_ = Distribucion::Uniforme;

    // Generador de numeros aleatorios moderno (reemplaza a rand()).
    std::mt19937 rng_;
};
