#include "LienzoSimulacion.h"
#include "Benchmark.h"

#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <chrono>
#include <iostream>

LienzoSimulacion::LienzoSimulacion(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(800, 600);
    setMouseTracking(true);

    sim_.configurar(/*n*/ 1000, /*ancho*/ 800, /*alto*/ 600,
                    /*capacidad*/ 4, /*radio*/ 3.0, /*velocidadMax*/ 1.0,
                    Distribucion::Uniforme);
    sim_.generar();

    connect(&timer_, &QTimer::timeout, this, &LienzoSimulacion::avanzarFrame);
    timer_.start(16); // ~60 FPS
}

void LienzoSimulacion::reiniciar()
{
    sim_.generar();
    update();
}

void LienzoSimulacion::establecerCantidad(int n)
{
    sim_.establecerCantidad(n);
    reiniciar();
}
void LienzoSimulacion::establecerCapacidad(int c)
{
    sim_.establecerCapacidad(c);
    reiniciar();
}
void LienzoSimulacion::establecerRadio(double r)
{
    sim_.establecerRadio(r);
    reiniciar();
}
void LienzoSimulacion::establecerVelocidad(double v)
{
    sim_.establecerVelocidadMax(v);
    reiniciar();
}
void LienzoSimulacion::establecerDistribucion(int idx)
{
    sim_.establecerDistribucion(static_cast<Distribucion>(idx));
    reiniciar();
}

void LienzoSimulacion::alternarPausa()
{
    pausado_ = !pausado_;
    if (pausado_)
        timer_.stop();
    else
        timer_.start(16);
}

void LienzoSimulacion::ejecutarBenchmark()
{
    std::cout << "\n>>> Ejecutando benchmark (puede tardar unos segundos)...\n";
    ::ejecutarBenchmark(std::cout);
    std::cout << ">>> Benchmark finalizado.\n";
}

void LienzoSimulacion::resizeEvent(QResizeEvent *evento)
{
    sim_.redimensionar(evento->size().width(), evento->size().height());
    QWidget::resizeEvent(evento);
}

void LienzoSimulacion::avanzarFrame()
{
    const auto t0 = std::chrono::high_resolution_clock::now();

    sim_.mover();
    arbol_ = sim_.construirArbol();

    sim_.resetColisiones();

    compararFB_ = (sim_.cantidad() <= 5000);
    if (compararFB_)
    {
        metFB_ = sim_.detectarFuerzaBruta(false);
    }
    else
    {
        metFB_ = Metricas{};
    }

    metQT_ = sim_.detectarConQuadTree(*arbol_, true);

    candidatos_.clear();
    if (mouseDentro_ && arbol_)
    {
        const double r = sim_.radio() * 8.0;
        const Punto centro{mousePos_.x(), mousePos_.y()};
        arbol_->buscarCercaDePunto(centro, r, candidatos_);
    }

    const double dt = std::chrono::duration<double>(
                          std::chrono::high_resolution_clock::now() - t0)
                          .count();
    fps_ = (dt > 0.0) ? (1.0 / dt) : 0.0;

    const double candPorParticula = sim_.cantidad() > 0
                                        ? static_cast<double>(metQT_.comparaciones) / sim_.cantidad()
                                        : 0.0;
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

    update();
}

void LienzoSimulacion::paintEvent(QPaintEvent *)
{
    QPainter pintor(this);
    pintor.setRenderHint(QPainter::Antialiasing, true);

    pintor.fillRect(rect(), QColor(18, 18, 24));

    if (arbol_)
    {
        std::vector<Rectangulo> limites;
        arbol_->obtenerLimites(limites);
        pintor.setBrush(Qt::NoBrush);
        pintor.setPen(QPen(QColor(70, 90, 120), 1));
        for (const Rectangulo &r : limites)
        {
            pintor.drawRect(QRectF(r.x - r.w, r.y - r.h, r.w * 2.0, r.h * 2.0));
        }
    }

    for (const Particula &p : sim_.particulas())
    {
        if (p.colisionando)
        {
            pintor.setBrush(QColor(235, 80, 80));
            pintor.setPen(QPen(QColor(255, 140, 140), 1));
        }
        else
        {
            pintor.setBrush(QColor(90, 180, 220));
            pintor.setPen(Qt::NoPen);
        }
        pintor.drawEllipse(QPointF(p.pos.x, p.pos.y), p.radio, p.radio);
    }

    if (mouseDentro_)
    {
        const double r = sim_.radio() * 8.0;
        pintor.setBrush(QColor(255, 220, 0, 35));
        pintor.setPen(QPen(QColor(255, 220, 0), 1, Qt::DashLine));
        pintor.drawRect(QRectF(mousePos_.x() - r, mousePos_.y() - r, r * 2.0, r * 2.0));

        pintor.setBrush(Qt::NoBrush);
        pintor.setPen(QPen(QColor(255, 235, 120), 2));
        for (Particula *c : candidatos_)
        {
            pintor.drawEllipse(QPointF(c->pos.x, c->pos.y), c->radio + 3.0, c->radio + 3.0);
        }
    }

    const double candPorParticula = sim_.cantidad() > 0
                                        ? static_cast<double>(metQT_.comparaciones) / sim_.cantidad()
                                        : 0.0;
    QStringList lineas;
    lineas << QString("FPS: %1").arg(fps_, 0, 'f', 0);
    lineas << QString("QuadTree: %1 comp / %2 ms")
                  .arg(metQT_.comparaciones)
                  .arg(metQT_.tiempoSegundos * 1000.0, 0, 'f', 2);
    if (compararFB_)
        lineas << QString("Fuerza Bruta: %1 comp / %2 ms")
                      .arg(metFB_.comparaciones)
                      .arg(metFB_.tiempoSegundos * 1000.0, 0, 'f', 2);
    else
        lineas << QString("Fuerza Bruta: omitida (n > 5000)");
    lineas << QString("Candidatos/particula: %1").arg(candPorParticula, 0, 'f', 2);

    // Opcion B: contador de la consulta interactiva (region del mouse).
    if (mouseDentro_)
        lineas << QString("Consulta mouse: %1 candidatos").arg(candidatos_.size());

    pintor.setPen(Qt::NoPen);
    pintor.setBrush(QColor(0, 0, 0, 150));
    pintor.drawRect(QRectF(8, 8, 320, 20.0 * lineas.size() + 10));
    pintor.setPen(QColor(235, 235, 235));
    int y = 26;
    for (const QString &l : lineas)
    {
        pintor.drawText(16, y, l);
        y += 20;
    }
}

void LienzoSimulacion::mouseMoveEvent(QMouseEvent *evento)
{
    mouseDentro_ = true;
    mousePos_ = evento->position();
    update();
}

void LienzoSimulacion::leaveEvent(QEvent *)
{
    mouseDentro_ = false;
    update();
}