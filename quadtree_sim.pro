# ============================================================================
#  quadtree_sim.pro
#  Proyecto qmake para la APLICACION GRAFICA (Qt6) por si prefieren abrirlo
#  directamente en Qt Creator. Para el benchmark de consola usar CMake o el
#  comando g++ que aparece en el README.
# ============================================================================

QT       += core gui widgets
CONFIG   += c++17
TEMPLATE  = app
TARGET    = quadtree_sim

INCLUDEPATH += include

# Nucleo (sin Qt)
SOURCES += src/QuadTree.cpp \
           src/Simulacion.cpp \
           src/Benchmark.cpp

# Capa grafica (Qt)
SOURCES += qt/LienzoSimulacion.cpp \
           qt/VentanaPrincipal.cpp \
           qt/main.cpp

HEADERS += include/Geometria.h \
           include/Particula.h \
           include/QuadTree.h \
           include/Simulacion.h \
           include/Benchmark.h \
           qt/LienzoSimulacion.h \
           qt/VentanaPrincipal.h
