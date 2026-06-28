cmake_minimum_required(VERSION 3.16)
project(QuadTreeParticulas LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt necesita estos automatismos para generar el codigo de moc/uic/rcc.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

# ---- Nucleo puro (sin Qt): se usa tanto en la GUI como en el benchmark ----
set(FUENTES_NUCLEO
    src/QuadTree.cpp
    src/Simulacion.cpp
    src/Benchmark.cpp
)

# ---- Ejecutable de consola para la comparacion experimental ----
# Siempre se puede compilar, incluso en maquinas sin Qt instalado.
add_executable(quadtree_bench
    ${FUENTES_NUCLEO}
    src/main_benchmark.cpp
)

# ---- Aplicacion grafica (Qt6). Solo se construye si Qt6 esta disponible ----
find_package(Qt6 COMPONENTS Widgets QUIET)
if (Qt6_FOUND)
    add_executable(quadtree_sim
        ${FUENTES_NUCLEO}
        qt/LienzoSimulacion.cpp
        qt/VentanaPrincipal.cpp
        qt/main.cpp
    )
    target_link_libraries(quadtree_sim PRIVATE Qt6::Widgets)
    message(STATUS "Qt6 encontrado: se construira la aplicacion grafica 'quadtree_sim'.")
else()
    message(WARNING "Qt6 no encontrado: solo se construira 'quadtree_bench'. "
                    "Instala Qt6 (modulo Widgets) para la aplicacion grafica.")
endif()
