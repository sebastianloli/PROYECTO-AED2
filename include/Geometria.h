#pragma once

// ============================================================================
//  Geometria.h
//  Tipos geometricos basicos usados por el QuadTree y la simulacion.
//  No dependen de Qt: forman parte del "nucleo" puro en C++17.
// ============================================================================

// Un punto en el plano 2D (posicion de una particula, centro de una region, etc.)
struct Punto {
    double x = 0.0;
    double y = 0.0;
};

// Rectangulo Axis-Aligned (AABB) definido por su CENTRO (x, y) y sus
// SEMI-DIMENSIONES (w = semiancho, h = semialto).
// Asi, el rectangulo cubre [x - w, x + w] x [y - h, y + h].
// Esta representacion por centro+semiejes facilita subdividir en 4 cuadrantes.
struct Rectangulo {
    double x = 0.0;   // centro X
    double y = 0.0;   // centro Y
    double w = 0.0;   // semiancho
    double h = 0.0;   // semialto

    // Devuelve true si el punto p cae dentro de este rectangulo (bordes incluidos).
    bool contiene(const Punto& p) const {
        return (p.x >= x - w && p.x <= x + w &&
                p.y >= y - h && p.y <= y + h);
    }

    // Devuelve true si este rectangulo se solapa con 'otro'.
    // Se usa para podar la busqueda: si la region consultada no toca este nodo,
    // ni siquiera entramos a revisarlo.
    bool choca(const Rectangulo& otro) const {
        return !(otro.x - otro.w > x + w ||   // 'otro' esta totalmente a la derecha
                 otro.x + otro.w < x - w ||   // 'otro' esta totalmente a la izquierda
                 otro.y - otro.h > y + h ||   // 'otro' esta totalmente abajo
                 otro.y + otro.h < y - h);    // 'otro' esta totalmente arriba
    }
};
