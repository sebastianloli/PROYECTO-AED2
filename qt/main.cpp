#include <QApplication>
#include "VentanaPrincipal.h"

// ============================================================================
//  main.cpp
//  Punto de entrada de la aplicacion grafica (Qt). Crea la ventana principal,
//  que a su vez contiene el lienzo de simulacion y los controles.
//
//  La comparacion experimental tambien puede ejecutarse desde aqui con el
//  boton "Ejecutar benchmark", o de forma independiente con el ejecutable de
//  consola 'quadtree_bench' (ver README).
// ============================================================================

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    VentanaPrincipal ventana;
    ventana.resize(1100, 640);
    ventana.show();
    return app.exec();
}
