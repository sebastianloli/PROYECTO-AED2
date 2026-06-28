# QuadTree — Simulador de Partículas 2D

Proyecto final de **CS2023 — Algoritmos y Estructuras de Datos** (Opción A: QuadTree).
Implementa un **QuadTree desde cero** y lo aplica a un simulador 2D de partículas en
movimiento, detectando colisiones/vecinos cercanos. Incluye una comparación
experimental contra la solución ingenua (fuerza bruta, O(n²)).

---

## 1. ¿Qué hace la aplicación?

- Simula `n` partículas circulares que se mueven y rebotan en los bordes.
- En cada frame: mueve las partículas, **reconstruye el QuadTree** y lo usa para
  detectar colisiones consultando solo la vecindad de cada partícula.
- Permite configurar todos los parámetros pedidos: número de partículas, tamaño del
  espacio (tamaño de la ventana), capacidad máxima por nodo, radio, velocidad y la
  distribución inicial.
- Implementa los **tres escenarios** requeridos: *uniforme*, *clusters* y *alta densidad*.
- Visualiza: las partículas, las **subdivisiones del QuadTree**, la **región consultada**
  bajo el cursor, los **candidatos** que devuelve el árbol, las **colisiones** detectadas
  (en rojo) y un panel con las **métricas** (comparaciones de QuadTree vs fuerza bruta,
  tiempos y candidatos por partícula).
- Incluye un programa de consola para la **comparación experimental** con varios tamaños.

---

## 2. ¿Por qué elegimos Qt para la visualización?

Evaluamos SFML, raylib, SDL y Qt (todas permitidas por el enunciado). Elegimos **Qt6**
por las siguientes razones:

1. **Un solo framework para interfaz + dibujo.** El proyecto necesita dos cosas al mismo
   tiempo: *controles interactivos* para los parámetros configurables (cantidad de
   partículas, capacidad del nodo, radio, velocidad, distribución) y un *render en tiempo
   real*. Qt trae ambas cosas de fábrica: los **widgets** (`QSpinBox`, `QComboBox`,
   `QPushButton`) para los controles y `QPainter` para el dibujo. SFML, raylib y SDL están
   orientados al *game-loop* y para tener controles habría que sumar otra librería
   (por ejemplo ImGui), agregando dependencias y complejidad.

2. **`QPainter` es ideal para esta visualización.** Dibujar los círculos de las partículas,
   los rectángulos de las subdivisiones del QuadTree, la región de consulta y los overlays
   de texto se resuelve con primitivas directas (`drawEllipse`, `drawRect`, `drawText`) y
   antialiasing integrado, con muy poco código.

3. **Bucle de frames limpio con `QTimer` y señales/slots.** El timer dispara el avance de
   cada frame sin tener que manejar manualmente el bucle ni los *sleeps*, y el mecanismo de
   **signals/slots** desacopla los controles de la lógica: cada cambio de parámetro emite
   una señal que regenera la simulación.

4. **Madurez, documentación y portabilidad.** Qt es multiplataforma (Windows, Linux,
   macOS), está muy documentado y disponible en los laboratorios, lo que reduce problemas
   de configuración.

5. **Experiencia previa del equipo.** Ya habíamos trabajado con Qt6 en un proyecto anterior,
   lo que nos permitió enfocar el tiempo en la estructura de datos y no en aprender el
   framework gráfico.

Importante: Qt se usa **solo para la interfaz y el dibujo**. El núcleo algorítmico —el
QuadTree y la detección de colisiones— está implementado por nosotros y **no depende de Qt**
(vive en `include/` y `src/`), tal como exige el enunciado.

---

## 3. Estructura del proyecto

```
quadtree-particulas/
├── include/                 # Núcleo (C++ puro, sin Qt)
│   ├── Geometria.h          # Punto y Rectángulo (AABB)
│   ├── Particula.h          # struct Particula + test de colisión
│   ├── QuadTree.h           # declaración del QuadTree
│   ├── Simulacion.h         # lógica de simulación y métricas
│   └── Benchmark.h          # comparación experimental
├── src/
│   ├── QuadTree.cpp
│   ├── Simulacion.cpp
│   ├── Benchmark.cpp
│   └── main_benchmark.cpp   # ejecutable de consola (experimentos)
├── qt/                      # Capa de visualización (Qt6)
│   ├── LienzoSimulacion.h/.cpp   # widget que dibuja la simulación
│   ├── VentanaPrincipal.h/.cpp   # ventana con controles
│   └── main.cpp                  # punto de entrada de la GUI
├── CMakeLists.txt           # build con CMake (GUI + benchmark)
├── quadtree_sim.pro         # build con qmake / Qt Creator (GUI)
├── README.md
└── JUSTIFICACION.md         # justificación de la estructura + complejidad
```

---

## 4. Requisitos

- Compilador **C++17** (g++ 9+, clang 10+ o MSVC 2019+).
- **Qt6** (módulo *Widgets*) para la aplicación gráfica.
- **CMake 3.16+** o **qmake** (cualquiera de los dos).

---

## 5. Compilación y ejecución

### Opción A — CMake (recomendada)

```bash
cd quadtree-particulas
cmake -S . -B build
cmake --build build

# Aplicación gráfica (si Qt6 está instalado):
./build/quadtree_sim

# Comparación experimental por consola:
./build/quadtree_bench
```

Si Qt6 **no** está instalado, CMake igual compila `quadtree_bench` (el benchmark no
necesita Qt) y avisa que omite la GUI.

### Opción B — Qt Creator / qmake (solo la GUI)

Abrir `quadtree_sim.pro` en Qt Creator y pulsar *Run*, o por consola:

```bash
qmake quadtree_sim.pro
make
./quadtree_sim
```

### Opción C — Solo el benchmark, sin CMake ni Qt

```bash
g++ -std=c++17 -O2 -Iinclude \
    src/QuadTree.cpp src/Simulacion.cpp src/Benchmark.cpp src/main_benchmark.cpp \
    -o quadtree_bench
./quadtree_bench
```

---

## 6. Uso de la aplicación gráfica

- **Panel izquierdo:** ajusta partículas, capacidad de nodo, radio, velocidad y la
  distribución (uniforme / clusters / alta densidad). Cada cambio regenera la simulación.
- **Pausar / Reanudar** y **Reiniciar** controlan la animación.
- **Mueve el mouse** sobre el lienzo para ver la **región de consulta** (amarillo) y los
  **candidatos** que devuelve el QuadTree (resaltados): así se observa la consulta por
  región / vecindad.
- El **panel de métricas** muestra en vivo las comparaciones de QuadTree vs fuerza bruta,
  los tiempos y los candidatos por partícula.
- **Ejecutar benchmark** corre la comparación experimental e imprime la tabla en la consola
  desde la que se lanzó la app.

> Nota: para `n > 5000`, la fuerza bruta (O(n²)) se desactiva automáticamente en la vista
> en vivo para no frenar la animación; la comparación completa con esos tamaños se hace en
> el benchmark.

---

## 7. Comparación experimental

`quadtree_bench` recorre los tamaños indicados y las tres distribuciones, y reporta:
tiempo de construcción del árbol, tiempo promedio por frame de cada método, comparaciones
promedio, candidatos por partícula y *speedup*.

```bash
./quadtree_bench                       # 1000, 5000, 10000 (15 frames)
./quadtree_bench 1000 5000 10000 20000 # tamaños personalizados
./quadtree_bench --frames 30 1000 5000 10000
```

---

## 8. Datos

La aplicación genera **datos sintéticos configurables**; no usa ningún dataset externo
(permitido por el enunciado para la opción QuadTree).

---

## 9. Autores

Curso CS2023 — Algoritmos y Estructuras de Datos · UTEC.
Repositorio: https://github.com/sebastianloli/PROYECTO-AED2
