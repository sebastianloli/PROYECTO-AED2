#include "QuadTree.h"

// ============================================================================
//  QuadTree.cpp
//  Implementacion de las operaciones del arbol cuaternario.
// ============================================================================

QuadTree::QuadTree(const Rectangulo& limite, int capacidad,
                   int profundidad, int profundidadMaxima)
    : limite_(limite),
      capacidad_(capacidad),
      profundidad_(profundidad),
      profundidadMaxima_(profundidadMaxima),
      dividido_(false) {}

QuadTree::~QuadTree() {
    // Solo hay hijos si el nodo se subdividio; los borramos recursivamente.
    if (dividido_) {
        delete noroeste_;
        delete noreste_;
        delete sudoeste_;
        delete sudeste_;
    }
}

void QuadTree::subdividir() {
    // El nodo se parte en 4 cuadrantes de la mitad de tamano.
    const double x = limite_.x;
    const double y = limite_.y;
    const double w = limite_.w / 2.0;
    const double h = limite_.h / 2.0;

    // Centros de cada cuadrante hijo.
    const Rectangulo no = {x - w, y - h, w, h}; // noroeste
    const Rectangulo ne = {x + w, y - h, w, h}; // noreste
    const Rectangulo so = {x - w, y + h, w, h}; // sudoeste
    const Rectangulo se = {x + w, y + h, w, h}; // sudeste

    noroeste_ = new QuadTree(no, capacidad_, profundidad_ + 1, profundidadMaxima_);
    noreste_  = new QuadTree(ne, capacidad_, profundidad_ + 1, profundidadMaxima_);
    sudoeste_ = new QuadTree(so, capacidad_, profundidad_ + 1, profundidadMaxima_);
    sudeste_  = new QuadTree(se, capacidad_, profundidad_ + 1, profundidadMaxima_);
    dividido_ = true;

    // Repartimos las particulas que YA estaban en este nodo hacia las hojas hijas.
    // (Asi mantenemos las particulas solo en las hojas, que es lo mas eficiente).
    std::vector<Particula*> previas;
    previas.swap(particulas_); // vaciamos particulas_ y nos quedamos con la copia local
    for (Particula* p : previas) {
        if (noroeste_->insertar(p)) continue;
        if (noreste_->insertar(p))  continue;
        if (sudoeste_->insertar(p)) continue;
        if (sudeste_->insertar(p))  continue;
        // Respaldo: si por bordes flotantes no encajo en ningun hijo, se queda aqui.
        particulas_.push_back(p);
    }
}

bool QuadTree::insertar(Particula* p) {
    // 1) Si la particula no cae dentro de la region de este nodo, no es nuestra.
    if (!limite_.contiene(p->pos)) {
        return false;
    }

    // 2) Si es una hoja con espacio (o ya llegamos a la profundidad maxima),
    //    la guardamos aqui. El tope de profundidad evita recursion infinita
    //    cuando muchas particulas comparten casi la misma posicion.
    if (!dividido_ &&
        (static_cast<int>(particulas_.size()) < capacidad_ ||
         profundidad_ >= profundidadMaxima_)) {
        particulas_.push_back(p);
        return true;
    }

    // 3) Si la hoja esta llena, la subdividimos (una sola vez).
    if (!dividido_) {
        subdividir();
    }

    // 4) Delegamos la insercion al cuadrante hijo que corresponda.
    if (noroeste_->insertar(p)) return true;
    if (noreste_->insertar(p))  return true;
    if (sudoeste_->insertar(p)) return true;
    if (sudeste_->insertar(p))  return true;

    // 5) Respaldo defensivo (bordes flotantes): la guardamos en este nodo.
    particulas_.push_back(p);
    return true;
}

void QuadTree::buscar(const Rectangulo& rango, std::vector<Particula*>& encontradas) const {
    // PODA: si el rango buscado no toca la region de este nodo, no hay nada que ver.
    if (!limite_.choca(rango)) {
        return;
    }

    // Revisamos las particulas guardadas en este nodo.
    for (Particula* p : particulas_) {
        if (rango.contiene(p->pos)) {
            encontradas.push_back(p);
        }
    }

    // Si el nodo esta subdividido, bajamos a los hijos que toquen el rango.
    if (dividido_) {
        noroeste_->buscar(rango, encontradas);
        noreste_->buscar(rango, encontradas);
        sudoeste_->buscar(rango, encontradas);
        sudeste_->buscar(rango, encontradas);
    }
}

void QuadTree::buscarCercaDePunto(const Punto& centro, double radio,
                                  std::vector<Particula*>& encontradas) const {
    // Construimos una ventana cuadrada centrada en el punto y reutilizamos buscar().
    const Rectangulo area = {centro.x, centro.y, radio, radio};
    buscar(area, encontradas);
}

void QuadTree::obtenerLimites(std::vector<Rectangulo>& lista) const {
    lista.push_back(limite_);
    if (dividido_) {
        noroeste_->obtenerLimites(lista);
        noreste_->obtenerLimites(lista);
        sudoeste_->obtenerLimites(lista);
        sudeste_->obtenerLimites(lista);
    }
}
