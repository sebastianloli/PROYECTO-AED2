#include "LienzoSimulacion.h"
#include "Benchmark.h"

#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <chrono>
#include <iostream>

// ============================================================================
//  LienzoSimulacion.cpp
// ============================================================================

LienzoSimulacion::LienzoSimulacion(QWidget* parent) : QWidget(parent) {
    setMinimumSize(800, 600);
    setMouseTracking(true); // recibir mouseMove aunque no haya boton presionado

    // Configuracion inicial de la simulacion.
    sim_.configurar(/*n*/1000, /*ancho*/800, /*alto*/600,
                    /*capacidad*/4, /*radio*/3.0, /*velocidadMax*/1.0,
                    Distribucion::Uniforme);
    sim_.generar();

    // El QTimer dispara avanzarFrame() ~60 veces por segundo.
    connect(&timer_, &QTimer::timeout, this, &LienzoSimulacion::avanzarFrame);
    timer_.start(16); // ~60 FPS
}

void LienzoSimulacion::reiniciar() {
    sim_.generar();
    update();
}

// ---- Slots de configuracion: cada cambio regenera la simulacion ----
void LienzoSimulacion::establecerCantidad(int n)        { sim_.establecerCantidad(n);        reiniciar(); }
void LienzoSimulacion::establecerCapacidad(int c)       { sim_.establecerCapacidad(c);       reiniciar(); }
void LienzoSimulacion::establecerRadio(double r)        { sim_.establecerRadio(r);           reiniciar(); }
void LienzoSimulacion::establecerVelocidad(double v)    { sim_.establecerVelocidadMax(v);    reiniciar(); }
void LienzoSimulacion::establecerDistribucion(int idx)  {
    sim_.establecerDistribucion(static_cast<Distribucion>(idx));
    reiniciar();
}

void LienzoSimulacion::alternarPausa() {
    pausado_ = !pausado_;
    if (pausado_) timer_.stop();
    else          timer_.start(16);
}

void LienzoSimulacion::ejecutarBenchmark() {
    // Corre la comparacion experimental en la consola desde donde se lanzo la app.
    std::cout << "\n>>> Ejecutando benchmark (puede tardar unos segundos)...\n";
    ::ejecutarBenchmark(std::cout); // funcion global de Benchmark.h (evita colision con este metodo)
    std::cout << ">>> Benchmark finalizado.\n";
}

void LienzoSimulacion::resizeEvent(QResizeEvent* evento) {
    // Hacemos que el mundo de la simulacion coincida con el tamano del widget,
    // asi el dibujo es 1:1 y el mouse mapea directo a coordenadas del mundo.
    sim_.redimensionar(evento->size().width(), evento->size().height());
    QWidget::resizeEvent(evento);
}

void LienzoSimulacion::avanzarFrame() {
    const auto t0 = std::chrono::high_resolution_clock::now();

    // 1) Mover particulas y 2) reconstruir el QuadTree (se mueven cada frame).
    sim_.mover();
    arbol_ = sim_.construirArbol();

    // 3) Limpiar marcas de colision antes de detectar.
    sim_.resetColisiones();

    // 4) Para n grande, la fuerza bruta (O(n^2)) frenaria la animacion, asi que
    //    la desactivamos automaticamente y lo avisamos en el panel.
    compararFB_ = (sim_.cantidad() <= 5000);
    if (compararFB_) {
        metFB_ = sim_.detectarFuerzaBruta(false); // solo medimos, no marcamos
    } else {
        metFB_ = Metricas{};
    }

    // 5) Deteccion con QuadTree: esta SI marca las colisiones que se dibujan.
    metQT_ = sim_.detectarConQuadTree(*arbol_, true);

    // 6) Consulta interactiva: candidatos cerca del cursor (region de busqueda).
    candidatos_.clear();
    if (mouseDentro_ && arbol_) {
        const double r = sim_.radio() * 8.0; // ventana de consulta visible
        const Punto centro{mousePos_.x(), mousePos_.y()};
        arbol_->buscarCercaDePunto(centro, r, candidatos_);
    }

    // FPS aproximado a partir del costo del frame.
    const double dt = std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now() - t0).count();
    fps_ = (dt > 0.0) ? (1.0 / dt) : 0.0;

    // Texto de metricas para el panel lateral.
    const double candPorParticula = sim_.cantidad() > 0
        ? static_cast<double>(metQT_.comparaciones) / sim_.cantidad() : 0.0;
    QString texto;
    texto += QString("Particulas: %1\n").arg(sim_.cantidad());
    texto += QString("Capacidad nodo: %1\n").arg(sim_.capacidadNodo());
    texto += QString("Comparaciones QuadTree: %1\n").arg(metQT_.comparaciones);
    if (compararFB_)
        texto += QString("Comparaciones Fuerza Bruta: %1\n").arg(metFB_.comparaciones);
    else
        texto += QString("Fuerza Bruta: omitida (n > 5000)\n");
    texto += QString("Candidatos por particula: %1\n").arg(candPorParticula, 0, 'f', 2);
    texto += QString("Tiempo QuadTree: %1 ms\n").arg(metQT_.tiempoSegundos * 1000.0, 0, 'f', 3);
    if (compararFB_)
        texto += QString("Tiempo Fuerza Bruta: %1 ms\n").arg(metFB_.tiempoSegundos * 1000.0, 0, 'f', 3);
    emit metricasActualizadas(texto);

    update(); // pedir repintado
}

void LienzoSimulacion::paintEvent(QPaintEvent*) {
    QPainter pintor(this);
    pintor.setRenderHint(QPainter::Antialiasing, true);

    // Fondo.
    pintor.fillRect(rect(), QColor(18, 18, 24));

    // ---- 1) Subdivisiones del QuadTree (rectangulos de cada nodo) ----
    if (arbol_) {
        std::vector<Rectangulo> limites;
        arbol_->obtenerLimites(limites);
        pintor.setBrush(Qt::NoBrush);
        pintor.setPen(QPen(QColor(70, 90, 120), 1));
        for (const Rectangulo& r : limites) {
            // Convertimos de centro+semiejes a esquina superior izquierda + tamano.
            pintor.drawRect(QRectF(r.x - r.w, r.y - r.h, r.w * 2.0, r.h * 2.0));
        }
    }

    // ---- 2) Particulas (rojo si colisionan, celeste si no) ----
    for (const Particula& p : sim_.particulas()) {
        if (p.colisionando) {
            pintor.setBrush(QColor(235, 80, 80));
            pintor.setPen(QPen(QColor(255, 140, 140), 1));
        } else {
            pintor.setBrush(QColor(90, 180, 220));
            pintor.setPen(Qt::NoPen);
        }
        pintor.drawEllipse(QPointF(p.pos.x, p.pos.y), p.radio, p.radio);
    }

    // ---- 3) Consulta interactiva bajo el cursor ----
    if (mouseDentro_) {
        const double r = sim_.radio() * 8.0;
        // Region rectangular consultada.
        pintor.setBrush(QColor(255, 220, 0, 35));
        pintor.setPen(QPen(QColor(255, 220, 0), 1, Qt::DashLine));
        pintor.drawRect(QRectF(mousePos_.x() - r, mousePos_.y() - r, r * 2.0, r * 2.0));

        // Candidatos devueltos por el QuadTree para esa region (resaltados).
        pintor.setBrush(Qt::NoBrush);
        pintor.setPen(QPen(QColor(255, 235, 120), 2));
        for (Particula* c : candidatos_) {
            pintor.drawEllipse(QPointF(c->pos.x, c->pos.y), c->radio + 3.0, c->radio + 3.0);
        }
    }

    // ---- 4) Panel de metricas (overlay arriba a la izquierda) ----
    const double candPorParticula = sim_.cantidad() > 0
        ? static_cast<double>(metQT_.comparaciones) / sim_.cantidad() : 0.0;
    QStringList lineas;
    lineas << QString("FPS: %1").arg(fps_, 0, 'f', 0);
    lineas << QString("QuadTree: %1 comp / %2 ms")
                  .arg(metQT_.comparaciones).arg(metQT_.tiempoSegundos * 1000.0, 0, 'f', 2);
    if (compararFB_)
        lineas << QString("Fuerza Bruta: %1 comp / %2 ms")
                      .arg(metFB_.comparaciones).arg(metFB_.tiempoSegundos * 1000.0, 0, 'f', 2);
    else
        lineas << QString("Fuerza Bruta: omitida (n > 5000)");
    lineas << QString("Candidatos/particula: %1").arg(candPorParticula, 0, 'f', 2);

    pintor.setPen(Qt::NoPen);
    pintor.setBrush(QColor(0, 0, 0, 150));
    pintor.drawRect(QRectF(8, 8, 320, 20.0 * lineas.size() + 10));
    pintor.setPen(QColor(235, 235, 235));
    int y = 26;
    for (const QString& l : lineas) { pintor.drawText(16, y, l); y += 20; }
}

void LienzoSimulacion::mouseMoveEvent(QMouseEvent* evento) {
    mouseDentro_ = true;
    // En Qt6, position() devuelve la posicion en coordenadas del widget (QPointF).
    mousePos_ = evento->position();
    update();
}

void LienzoSimulacion::leaveEvent(QEvent*) {
    mouseDentro_ = false;
    update();
}
