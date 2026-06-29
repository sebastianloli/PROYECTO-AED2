#pragma once

#include <vector>
#include "Geometria.h"
#include "Particula.h"

//  Arbol cuaternario para indexar particulas en el plano 2D.
//  Idea: cada nodo cubre una region rectangular. Si en esa region caben
//  como maximo 'capacidad' particulas, se quedan en el nodo (hoja). Cuando
//  se supera la capacidad, el nodo se SUBDIVIDE en 4 hijos (NO, NE, SO, SE)
//  y reparte sus particulas.

class QuadTree
{
public:
    QuadTree(const Rectangulo &limite,
             int capacidad,
             int profundidad = 0,
             int profundidadMaxima = 12);

    ~QuadTree();

    QuadTree(const QuadTree &) = delete;
    QuadTree &operator=(const QuadTree &) = delete;

    bool insertar(Particula *p);

    void buscar(const Rectangulo &rango, std::vector<Particula *> &encontradas) const;

    void buscarCercaDePunto(const Punto &centro, double radio,
                            std::vector<Particula *> &encontradas) const;

    void obtenerLimites(std::vector<Rectangulo> &lista) const;

    const Rectangulo &limite() const { return limite_; }
    bool dividido() const { return dividido_; }

private:
    void subdividir();

    Rectangulo limite_;
    int capacidad_;
    int profundidad_;
    int profundidadMaxima_;
    bool dividido_;

    std::vector<Particula *> particulas_;

    QuadTree *noroeste_ = nullptr;
    QuadTree *noreste_ = nullptr;
    QuadTree *sudoeste_ = nullptr;
    QuadTree *sudeste_ = nullptr;
};
