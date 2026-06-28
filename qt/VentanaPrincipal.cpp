#include "VentanaPrincipal.h"
#include "LienzoSimulacion.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>

//  Arma la interfaz y cablea los controles con el lienzo via signals/slots.

VentanaPrincipal::VentanaPrincipal(QWidget *parent) : QWidget(parent)
{
        setWindowTitle("QuadTree - Simulador de Particulas 2D");

        lienzo_ = new LienzoSimulacion(this);

        auto *spinN = new QSpinBox;
        spinN->setRange(10, 50000);
        spinN->setValue(1000);
        spinN->setSingleStep(100);

        auto *spinCap = new QSpinBox;
        spinCap->setRange(1, 64);
        spinCap->setValue(4);

        auto *spinRadio = new QDoubleSpinBox;
        spinRadio->setRange(1.0, 20.0);
        spinRadio->setValue(3.0);
        spinRadio->setSingleStep(0.5);

        auto *spinVel = new QDoubleSpinBox;
        spinVel->setRange(0.0, 10.0);
        spinVel->setValue(1.0);
        spinVel->setSingleStep(0.5);

        auto *comboDist = new QComboBox;
        comboDist->addItem("Uniforme");
        comboDist->addItem("Clusters");
        comboDist->addItem("Alta densidad");

        auto *btnPausa = new QPushButton("Pausar / Reanudar");
        auto *btnReiniciar = new QPushButton("Reiniciar");
        auto *btnBench = new QPushButton("Ejecutar benchmark (consola)");

        // Formulario con los parametros.
        auto *form = new QFormLayout;
        form->addRow("Particulas (n):", spinN);
        form->addRow("Capacidad nodo:", spinCap);
        form->addRow("Radio:", spinRadio);
        form->addRow("Velocidad max:", spinVel);
        form->addRow("Distribucion:", comboDist);

        auto *grupoParams = new QGroupBox("Parametros");
        grupoParams->setLayout(form);

        // Panel de metricas en vivo.
        panelMetricas_ = new QLabel("Metricas...");
        panelMetricas_->setWordWrap(true);
        panelMetricas_->setStyleSheet("font-family: monospace;");
        auto *grupoMetricas = new QGroupBox("Metricas");
        auto *layMetricas = new QVBoxLayout;
        layMetricas->addWidget(panelMetricas_);
        grupoMetricas->setLayout(layMetricas);

        // Columna de controles.
        auto *columna = new QVBoxLayout;
        columna->addWidget(grupoParams);
        columna->addWidget(btnPausa);
        columna->addWidget(btnReiniciar);
        columna->addWidget(btnBench);
        columna->addWidget(grupoMetricas);
        columna->addStretch();

        auto *ayudaQuery = new QLabel(
            "Mueve el mouse sobre el lienzo para ver la\n"
            "region de consulta (amarillo) y los candidatos\n"
            "que devuelve el QuadTree (resaltados).");
        ayudaQuery->setStyleSheet("color: gray;");
        columna->addWidget(ayudaQuery);

        // Layout principal: controles | lienzo.
        auto *principal = new QHBoxLayout(this);
        auto *contenedorControles = new QWidget;
        contenedorControles->setLayout(columna);
        contenedorControles->setFixedWidth(280);
        principal->addWidget(contenedorControles);
        principal->addWidget(lienzo_, /*stretch*/ 1);

        // ---------------- Conexiones (signals/slots) ----------------
        connect(spinN, qOverload<int>(&QSpinBox::valueChanged),
                lienzo_, &LienzoSimulacion::establecerCantidad);
        connect(spinCap, qOverload<int>(&QSpinBox::valueChanged),
                lienzo_, &LienzoSimulacion::establecerCapacidad);
        connect(spinRadio, qOverload<double>(&QDoubleSpinBox::valueChanged),
                lienzo_, &LienzoSimulacion::establecerRadio);
        connect(spinVel, qOverload<double>(&QDoubleSpinBox::valueChanged),
                lienzo_, &LienzoSimulacion::establecerVelocidad);
        connect(comboDist, qOverload<int>(&QComboBox::currentIndexChanged),
                lienzo_, &LienzoSimulacion::establecerDistribucion);
        connect(btnPausa, &QPushButton::clicked,
                lienzo_, &LienzoSimulacion::alternarPausa);
        connect(btnReiniciar, &QPushButton::clicked,
                lienzo_, &LienzoSimulacion::reiniciar);
        connect(btnBench, &QPushButton::clicked,
                lienzo_, &LienzoSimulacion::ejecutarBenchmark);

        connect(lienzo_, &LienzoSimulacion::metricasActualizadas,
                panelMetricas_, &QLabel::setText);
}
