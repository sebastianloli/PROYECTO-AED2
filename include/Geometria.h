#pragma once

//  Tipos geometricos basicos usados por el QuadTree y la simulacion.

struct Punto
{
    double x = 0.0;
    double y = 0.0;
};

struct Rectangulo
{
    double x = 0.0; // centro X
    double y = 0.0; // centro Y
    double w = 0.0; // semiancho
    double h = 0.0; // semialto

    // Devuelve true si el punto p cae dentro de este rectangulo.
    bool contiene(const Punto &p) const
    {
        return (p.x >= x - w && p.x <= x + w &&
                p.y >= y - h && p.y <= y + h);
    }

    bool choca(const Rectangulo &otro) const
    {
        return !(otro.x - otro.w > x + w || // 'otro' esta totalmente a la derecha
                 otro.x + otro.w < x - w || // 'otro' esta totalmente a la izquierda
                 otro.y - otro.h > y + h || // 'otro' esta totalmente abajo
                 otro.y + otro.h < y - h);  // 'otro' esta totalmente arriba
    }
};
