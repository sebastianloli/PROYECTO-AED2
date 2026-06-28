#pragma once

#include <QWidget>

class LienzoSimulacion;
class QLabel;

//  Ventana con el panel de controles (parametros configurables) a la izquierda
//  y el lienzo de simulacion a la derecha. Conecta cada control con el lienzo.

class VentanaPrincipal : public QWidget
{
    Q_OBJECT
public:
    explicit VentanaPrincipal(QWidget *parent = nullptr);

private:
    LienzoSimulacion *lienzo_ = nullptr;
    QLabel *panelMetricas_ = nullptr;
};
