#pragma once

#include <vector>
#include "Geometria.h"
#include "Particula.h"

// ============================================================================
//  QuadTree.h
//  Arbol cuaternario para indexar particulas en el plano 2D.
//  Implementado DESDE CERO (requisito del enunciado): no usa std::map/set
//  ni ninguna estructura espacial de libreria.
//
//  Idea: cada nodo cubre una region rectangular. Si en esa region caben
//  como maximo 'capacidad' particulas, se quedan en el nodo (hoja). Cuando
//  se supera la capacidad, el nodo se SUBDIVIDE en 4 hijos (NO, NE, SO, SE)
//  y reparte sus particulas. Asi, una consulta por region solo visita los
//  cuadrantes que tocan la region buscada, evitando revisar todo el mundo.
// ============================================================================

class QuadTree {
public:
    // limite: region que cubre este nodo.
    // capacidad: cuantas particulas admite una hoja antes de subdividirse.
    // profundidad / profundidadMaxima: limite de recursion para evitar
    //   subdivisiones infinitas cuando muchas particulas comparten posicion
    //   (caso de "alta densidad" en un mismo punto).
    QuadTree(const Rectangulo& limite,
             int capacidad,
             int profundidad = 0,
             int profundidadMaxima = 12);

    // Libera recursivamente los 4 hijos (manejo explicito de memoria).
    ~QuadTree();

    // Evitamos copias accidentales del arbol (manejaria mal los punteros crudos).
    QuadTree(const QuadTree&) = delete;
    QuadTree& operator=(const QuadTree&) = delete;

    // Inserta el puntero a una particula. Devuelve true si quedo dentro del arbol.
    bool insertar(Particula* p);

    // Devuelve en 'encontradas' todas las particulas cuyo centro cae dentro de 'rango'.
    void buscar(const Rectangulo& rango, std::vector<Particula*>& encontradas) const;

    // Conveniencia: busca particulas cercanas a un punto dentro de un radio.
    void buscarCercaDePunto(const Punto& centro, double radio,
                            std::vector<Particula*>& encontradas) const;

    // Devuelve los rectangulos de TODOS los nodos del arbol.
    // Lo usa la visualizacion para dibujar las subdivisiones actuales.
    void obtenerLimites(std::vector<Rectangulo>& lista) const;

    const Rectangulo& limite() const { return limite_; }
    bool dividido() const { return dividido_; }

private:
    // Crea los 4 hijos y reparte en ellos las particulas que tenia este nodo.
    void subdividir();

    Rectangulo limite_;            // region cubierta por este nodo
    int        capacidad_;         // capacidad maxima de una hoja
    int        profundidad_;       // nivel del nodo (0 = raiz)
    int        profundidadMaxima_; // limite de recursion
    bool       dividido_;          // true si ya tiene hijos

    std::vector<Particula*> particulas_; // particulas guardadas en este nodo (si es hoja)

    // Los 4 cuadrantes hijos. nullptr mientras el nodo no se haya subdividido.
    QuadTree* noroeste_ = nullptr;
    QuadTree* noreste_  = nullptr;
    QuadTree* sudoeste_ = nullptr;
    QuadTree* sudeste_  = nullptr;
};
