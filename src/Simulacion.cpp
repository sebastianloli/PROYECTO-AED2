#include "Simulacion.h"
#include <chrono>
#include <algorithm>

namespace
{
    using Reloj = std::chrono::high_resolution_clock;
    inline double segundosDesde(const Reloj::time_point &t0)
    {
        return std::chrono::duration<double>(Reloj::now() - t0).count();
    }
}

Simulacion::Simulacion()
{
    std::random_device sd;
    rng_.seed(sd());
}

void Simulacion::configurar(int n, double ancho, double alto,
                            int capacidadNodo, double radio, double velocidadMax,
                            Distribucion distrib)
{
    n_ = n;
    ancho_ = ancho;
    alto_ = alto;
    capacidadNodo_ = capacidadNodo;
    radio_ = radio;
    velocidadMax_ = velocidadMax;
    distribucion_ = distrib;
}

void Simulacion::redimensionar(double ancho, double alto)
{
    if (ancho <= 0 || alto <= 0)
        return;
    ancho_ = ancho;
    alto_ = alto;
    for (auto &p : particulas_)
    {
        p.pos.x = std::min(std::max(p.pos.x, p.radio), ancho_ - p.radio);
        p.pos.y = std::min(std::max(p.pos.y, p.radio), alto_ - p.radio);
    }
}

void Simulacion::generar()
{
    particulas_.clear();
    particulas_.reserve(n_);

    std::uniform_real_distribution<double> distX(radio_, ancho_ - radio_);
    std::uniform_real_distribution<double> distY(radio_, alto_ - radio_);
    std::uniform_real_distribution<double> distVel(-velocidadMax_, velocidadMax_);

    for (int i = 0; i < n_; ++i)
    {
        Particula p;
        p.id = i;
        p.radio = radio_;
        p.vx = distVel(rng_);
        p.vy = distVel(rng_);
        p.colisionando = false;

        switch (distribucion_)
        {
        case Distribucion::Uniforme:
        {
            p.pos.x = distX(rng_);
            p.pos.y = distY(rng_);
            break;
        }
        case Distribucion::Clusters:
        {
            const int numClusters = 4;
            std::uniform_int_distribution<int> cluster(0, numClusters - 1);
            std::uniform_real_distribution<double> desvio(-50.0, 50.0);
            // Centro del cluster elegido, repartido en una grilla del mundo.
            const double cx = (cluster(rng_) + 0.5) * (ancho_ / numClusters);
            const double cy = (cluster(rng_) + 0.5) * (alto_ / numClusters);
            p.pos.x = std::min(std::max(cx + desvio(rng_), radio_), ancho_ - radio_);
            p.pos.y = std::min(std::max(cy + desvio(rng_), radio_), alto_ - radio_);
            break;
        }
        case Distribucion::AltaDensidad:
        {
            std::bernoulli_distribution enCentro(0.8);
            if (enCentro(rng_))
            {
                std::uniform_real_distribution<double> cerca(-100.0, 100.0);
                p.pos.x = std::min(std::max(ancho_ / 2 + cerca(rng_), radio_), ancho_ - radio_);
                p.pos.y = std::min(std::max(alto_ / 2 + cerca(rng_), radio_), alto_ - radio_);
            }
            else
            { // 20% dispersas
                p.pos.x = distX(rng_);
                p.pos.y = distY(rng_);
            }
            break;
        }
        }
        particulas_.push_back(p);
    }
}

void Simulacion::mover()
{
    for (auto &p : particulas_)
    {
        p.pos.x += p.vx;
        p.pos.y += p.vy;

        if (p.pos.x - p.radio < 0)
        {
            p.pos.x = p.radio;
            p.vx = -p.vx;
        }
        else if (p.pos.x + p.radio > ancho_)
        {
            p.pos.x = ancho_ - p.radio;
            p.vx = -p.vx;
        }

        if (p.pos.y - p.radio < 0)
        {
            p.pos.y = p.radio;
            p.vy = -p.vy;
        }
        else if (p.pos.y + p.radio > alto_)
        {
            p.pos.y = alto_ - p.radio;
            p.vy = -p.vy;
        }
    }
}

std::unique_ptr<QuadTree> Simulacion::construirArbol()
{
    const Rectangulo mundo = {ancho_ / 2.0, alto_ / 2.0, ancho_ / 2.0, alto_ / 2.0};
    auto arbol = std::make_unique<QuadTree>(mundo, capacidadNodo_);
    for (auto &p : particulas_)
    {
        arbol->insertar(&p);
    }
    return arbol;
}

void Simulacion::resetColisiones()
{
    for (auto &p : particulas_)
    {
        p.colisionando = false;
    }
}

Metricas Simulacion::detectarFuerzaBruta(bool registrar)
{
    Metricas m;
    const auto t0 = Reloj::now();
    for (auto &a : particulas_)
    {
        for (auto &b : particulas_)
        {
            if (a.id == b.id)
                continue;
            ++m.comparaciones;
            if (estanChocando(a, b) && registrar)
            {
                a.colisionando = true;
            }
        }
    }
    m.tiempoSegundos = segundosDesde(t0);
    return m;
}

Metricas Simulacion::detectarConQuadTree(const QuadTree &arbol, bool registrar)
{
    Metricas m;
    std::vector<Particula *> vecinos;
    const auto t0 = Reloj::now();
    for (auto &p : particulas_)
    {
        vecinos.clear();
        const Rectangulo zona = {p.pos.x, p.pos.y, p.radio * 2.0, p.radio * 2.0};
        arbol.buscar(zona, vecinos);

        for (Particula *v : vecinos)
        {
            if (p.id == v->id)
                continue;
            ++m.comparaciones;
            if (estanChocando(p, *v) && registrar)
            {
                p.colisionando = true;
            }
        }
    }
    m.tiempoSegundos = segundosDesde(t0);
    return m;
}
