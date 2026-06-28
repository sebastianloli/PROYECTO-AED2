#include <QApplication>
#include "VentanaPrincipal.h"

//  Punto de entrada de la aplicacion grafica (Qt). Crea la ventana principal, que a su vez contiene el lienzo de simulacion y los controles.

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    VentanaPrincipal ventana;
    ventana.resize(1100, 640);
    ventana.show();
    return app.exec();
}
