# Practica 4 - Técnica de Paso de Testigo
## Programación de sistemas concurrentes y distribuidos
Consulta el [enunciado de la practica](enunciado.pdf)

Este proyecto implementa un **sistema concurrente de gestión de publicidad** en C++ utilizando la técnica de **Paso de Testigo** para la sincronización de procesos mediante semáforos (`Semaphore_V4`). El sistema simula la interacción entre múltiples clientes que solicitan publicar imágenes y varios procesos publicadores encargados de procesar dichas solicitudes a través de colas con capacidad limitada.

### Arquitectura del sistema
El sistema está compuesto por tres tipos de procesos (hilos) concurrentes [0.1.3, cite: 1]:
- **Clientes (`NUM_Cl = 10`):** Generan peticiones de publicación de imágenes de forma aleatoria y esperan la respuesta del publicador asignado.
- **Publicadores (`NUM_Pub = 3`):** Atienden las colas de peticiones (Vallas Grandes y Vallas Pequeñas), determinan si aceptan la publicidad y devuelven una respuesta con el precio calculado.
- **Finalizador (1):** Controla el ciclo de vida, esperando a que todos los clientes terminen para enviar las señales de parada (`Finalizar`) a los publicadores [0.1.2, 0.1.3, cite: 1].

### Estructura de paso de testigo
Para garantizar la exclusión mutua y resolver las secciones críticas condicionales (`<await B S>`), se han implementado múltiples exclusiones independientes para maximizar la concurrencia:

1. **Gestión de Colas (`colaVG` y `colaVP`):**
   - Testigos principales: `testigoColaVG` y `testigocolaVP`.
   - Semáforos de condición: `b1`, `b2`, `b3` para controlar colas llenas y colas vacías.
2. **Sincronización de Respuestas:**
   - Uso de un array de semáforos privados `b[NUM_Cl]` y contadores de retraso `d[NUM_Cl]` para que cada cliente espere únicamente su propia respuesta de forma eficiente.
3. **Control del Finalizador:**
   - Evaluado mediante la condición del número de clientes finalizados (`numClientFin`).

### Requisitos previos
Para compilar y ejecutar el proyecto se necesitan los siguientes archivos de soporte en el mismo directorio:
- `Semaphore_V4.hpp` y `Semaphore_V4.cpp` (Clase suministrada para soporte de semáforos en C++11).
- `BoundedQueue.hpp` (Tipo de dato abstracto para la cola con capacidad limitada).

