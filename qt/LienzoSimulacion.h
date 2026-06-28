#pragma once

#include <QWidget>
#include <QTimer>
#include <QPointF>
#include <memory>
#include <vector>

#include "Simulacion.h"
#include "QuadTree.h"

// ============================================================================
//  LienzoSimulacion.h
//  Widget de Qt que dibuja la simulacion en tiempo real. Es la unica clase
//  que conoce el QuadTree y la Simulacion; toda la logica vive en el nucleo.
//
//  En cada frame: mueve las particulas, reconstruye el QuadTree, corre la
//  deteccion por QuadTree (y la de fuerza bruta para comparar) y se repinta.
//  Dibuja: particulas, subdivisiones del arbol, region de consulta bajo el
//  mouse, candidatos devueltos, colisiones y un panel de metricas.
// ============================================================================

class LienzoSimulacion : public QWidget {
    Q_OBJECT
public:
    explicit LienzoSimulacion(QWidget* parent = nullptr);

    // Regenera las particulas con los parametros actuales.
    void reiniciar();

public slots:
    // Slots conectados a los controles de la ventana principal.
    void establecerCantidad(int n);
    void establecerCapacidad(int c);
    void establecerRadio(double r);
    void establecerVelocidad(double v);
    void establecerDistribucion(int indice); // 0=Uniforme,1=Clusters,2=AltaDensidad
    void alternarPausa();
    void ejecutarBenchmark();                 // corre la comparacion en consola

signals:
    // Emite el texto de metricas para mostrarlo en un panel de la ventana.
    void metricasActualizadas(const QString& texto);

protected:
    void paintEvent(QPaintEvent* evento) override;   // dibujo
    void mouseMoveEvent(QMouseEvent* evento) override;// region de consulta
    void leaveEvent(QEvent* evento) override;         // mouse fuera del lienzo
    void resizeEvent(QResizeEvent* evento) override;  // ajustar mundo al tamano

private slots:
    void avanzarFrame(); // un paso de simulacion (lo dispara el QTimer)

private:
    Simulacion sim_;                       // nucleo de la simulacion
    std::unique_ptr<QuadTree> arbol_;      // arbol del frame actual (para dibujar)
    QTimer timer_;                         // reloj de frames

    Metricas metFB_;                       // metricas de fuerza bruta del frame
    Metricas metQT_;                       // metricas de quadtree del frame
    bool compararFB_ = true;               // se desactiva sola si n es muy grande

    bool   mouseDentro_ = false;           // hay consulta bajo el cursor?
    QPointF mousePos_;                     // posicion del cursor (en coords del mundo)
    std::vector<Particula*> candidatos_;   // candidatos devueltos por el QuadTree

    bool   pausado_ = false;
    double fps_ = 0.0;
};
