#pragma once

#include <QWidget>
#include <QTimer>
#include <QPointF>
#include <memory>
#include <vector>

#include "Simulacion.h"
#include "QuadTree.h"

//  Widget de Qt que dibuja la simulacion en tiempo real. Es la unica clase que conoce el QuadTree y la Simulacion; toda la logica vive en el nucleo.

class LienzoSimulacion : public QWidget
{
    Q_OBJECT
public:
    explicit LienzoSimulacion(QWidget *parent = nullptr);

    void reiniciar();

public slots:
    void establecerCantidad(int n);
    void establecerCapacidad(int c);
    void establecerRadio(double r);
    void establecerVelocidad(double v);
    void establecerDistribucion(int indice);
    void alternarPausa();
    void ejecutarBenchmark();

signals:
    void metricasActualizadas(const QString &texto);

protected:
    void paintEvent(QPaintEvent *evento) override;
    void mouseMoveEvent(QMouseEvent *evento) override;
    void leaveEvent(QEvent *evento) override;
    void resizeEvent(QResizeEvent *evento) override;

private slots:
    void avanzarFrame();

private:
    Simulacion sim_;
    std::unique_ptr<QuadTree> arbol_;
    QTimer timer_;

    Metricas metFB_;
    Metricas metQT_;
    bool compararFB_ = true;

    bool mouseDentro_ = false;
    QPointF mousePos_;
    std::vector<Particula *> candidatos_;

    bool pausado_ = false;
    double fps_ = 0.0;
};
