#pragma once

#include "Geometria.h"

// ============================================================================
//  Particula.h
//  Modelo de objeto que pide el enunciado (Opcion A: simulador de particulas).
//  Cada particula tiene id, posicion, velocidad y radio. El campo extra
//  'colisionando' se usa solo para la visualizacion (cambiar de color).
// ============================================================================

struct Particula {
    int    id          = 0;       // identificador unico
    Punto  pos;                   // posicion (x, y) en el mundo
    double vx          = 0.0;     // velocidad en X (px por frame)
    double vy          = 0.0;     // velocidad en Y (px por frame)
    double radio       = 3.0;     // radio del circulo
    bool   colisionando = false;  // true si choco con alguien este frame (para pintar)
};

// Test de colision entre dos circulos.
// En vez de calcular la distancia con sqrt (caro), comparamos las distancias
// AL CUADRADO contra la suma de radios al cuadrado: matematicamente equivalente
// y mas rapido.
inline bool estanChocando(const Particula& a, const Particula& b) {
    if (a.id == b.id) return false;            // una particula no choca consigo misma
    const double dx = a.pos.x - b.pos.x;
    const double dy = a.pos.y - b.pos.y;
    const double distanciaSq = dx * dx + dy * dy;
    const double sumaRadios  = a.radio + b.radio;
    return distanciaSq <= (sumaRadios * sumaRadios);
}
