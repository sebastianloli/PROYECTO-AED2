#pragma once

#include <vector>
#include <memory>
#include <random>
#include "Particula.h"
#include "QuadTree.h"

//  Mantiene la lista de particulas y ofrece:
//    - generacion de los 3 escenarios de distribucion,
//    - movimiento (un solo metodo, con rebote en bordes),
//    - construccion del QuadTree,
//    - deteccion de colisiones por fuerza bruta y por QuadTree,
//    - todos los parametros configurables que pide el enunciado.

enum class Distribucion
{
    Uniforme = 0,
    Clusters = 1,
    AltaDensidad = 2
};

struct Metricas
{
    long long comparaciones = 0;
    double tiempoSegundos = 0.0;
};

class Simulacion
{
public:
    Simulacion();

    void configurar(int n, double ancho, double alto,
                    int capacidadNodo, double radio, double velocidadMax,
                    Distribucion distrib);

    void establecerCantidad(int n) { n_ = n; }
    void establecerCapacidad(int c) { capacidadNodo_ = c; }
    void establecerRadio(double r) { radio_ = r; }
    void establecerVelocidadMax(double v) { velocidadMax_ = v; }
    void establecerDistribucion(Distribucion d) { distribucion_ = d; }

    // Ajusta el tamano del mundo (p. ej. al redimensionar la ventana Qt)
    void redimensionar(double ancho, double alto);

    // (Re)genera las particulas segun la distribucion y parametros actuales.
    void generar();

    void mover();

    // Construye un QuadTree nuevo con las particulas actuales.
    std::unique_ptr<QuadTree> construirArbol();

    void resetColisiones();

    Metricas detectarFuerzaBruta(bool registrar);

    Metricas detectarConQuadTree(const QuadTree &arbol, bool registrar);

    std::vector<Particula> &particulas() { return particulas_; }
    const std::vector<Particula> &particulas() const { return particulas_; }
    double ancho() const { return ancho_; }
    double alto() const { return alto_; }
    int cantidad() const { return n_; }
    int capacidadNodo() const { return capacidadNodo_; }
    double radio() const { return radio_; }

private:
    std::vector<Particula> particulas_;

    int n_ = 1000;
    double ancho_ = 800.0;
    double alto_ = 600.0;
    int capacidadNodo_ = 4;
    double radio_ = 3.0;
    double velocidadMax_ = 1.0;
    Distribucion distribucion_ = Distribucion::Uniforme;

    std::mt19937 rng_;
};
