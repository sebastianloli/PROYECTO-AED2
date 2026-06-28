# Justificación de la estructura y análisis de complejidad

**Proyecto:** QuadTree — Simulador de partículas 2D · CS2023

## Estructura elegida: QuadTree

Un **QuadTree** es un árbol cuaternario que particiona recursivamente el plano. Cada nodo
cubre una región rectangular y, cuando contiene más de `c` partículas (capacidad del nodo),
se **subdivide** en cuatro cuadrantes (NO, NE, SO, SE), repartiendo sus partículas entre
ellos. Es la estructura natural para el problema de detección de colisiones/vecinos en 2D,
porque agrupa espacialmente los objetos: para saber con quién puede chocar una partícula,
basta consultar su vecindad en lugar de revisar todo el espacio.

### Invariantes

- Cada partícula pertenece a la región (nodo) que la contiene.
- Una hoja almacena a lo sumo `c` partículas; al superarlas, se subdivide y reparte.
- Los cuatro hijos parten exactamente la región del padre (sin huecos).
- Se limita la profundidad para evitar subdivisión infinita cuando muchas partículas
  comparten posición (escenario de alta densidad).

### Operaciones implementadas

`insertar(p)`, `buscar(rango)` (consulta por región rectangular),
`buscarCercaDePunto(p, r)` (vecindad de un punto) y `obtenerLimites()` (para visualizar las
subdivisiones). La detección de colisiones por frame consulta, para cada partícula, una
ventana de tamaño proporcional a su radio y solo compara contra los **candidatos**
devueltos.

## Análisis de complejidad

Sean `n` el número de partículas, `c` la capacidad por nodo y `k` la cantidad de elementos
devueltos por una consulta. Con partículas razonablemente distribuidas el árbol queda
balanceado y su altura es **O(log n)**.

| Operación                                   | Promedio        | Peor caso |
|---------------------------------------------|-----------------|-----------|
| Insertar una partícula                      | O(log n)        | O(n)      |
| Construir el árbol (n inserciones)          | O(n log n)      | O(n²)     |
| Consulta por región / vecindad             | O(log n + k)    | O(n)      |
| Detección de colisiones (1 frame, todas)    | O(n log n + K)  | O(n²)     |
| **Solución ingenua (fuerza bruta)**         | **O(n²)**       | O(n²)     |

Donde `K` es el total de candidatos revisados en el frame. El **peor caso** O(n²) ocurre
cuando casi todas las partículas caen en la misma región (alta densidad extrema), porque el
árbol degenera; el límite de profundidad acota ese caso.

## Por qué mejora a la solución ingenua

La fuerza bruta compara cada partícula contra todas las demás: exactamente `n·(n−1)`
comparaciones por frame, es decir **O(n²)**. El QuadTree, en cambio, solo compara cada
partícula contra los pocos candidatos de su vecindad. Empíricamente medimos un número de
**candidatos por partícula** casi constante (≈ 0.1–2.5 según la distribución), de modo que
el trabajo total crece de forma aproximadamente **lineal** con `n`. Esto se confirma en los
experimentos: el *speedup* del QuadTree frente a la fuerza bruta **aumenta** con `n`, y es
mayor en distribuciones dispersas (uniforme) que en alta densidad, donde los nodos se llenan
más y el árbol pierde parte de su capacidad de poda.
