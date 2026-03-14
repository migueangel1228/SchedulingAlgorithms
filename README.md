# CPU Scheduler

Simulador de planificacion de CPU en C++17 con los algoritmos:

- FCFS
- SJF
- STCF / SRTF
- Round Robin

El programa lee procesos desde un archivo de texto, ejecuta cada algoritmo y muestra la secuencia de ejecucion junto con metricas basicas.

## Compilacion

Compila el programa con `g++`:

```bash
g++ -std=c++17 scheduler.cpp -o scheduler
```

En Windows con MinGW:

```powershell
g++ -std=c++17 scheduler.cpp -o scheduler.exe
```

## Ejecucion

Uso general:

```bash
./scheduler <input_file> [--verbose|-v] [-q N|--quantum N]
```

Ejemplos:

```bash
./scheduler input_example1.txt
./scheduler input_example2.txt --verbose
./scheduler -q 4 input_example3.txt
./scheduler --quantum 2 input_example1.txt -v
./scheduler --help
```

En Windows:

```powershell
.\scheduler.exe input_example1.txt
.\scheduler.exe -q 4 input_example2.txt
.\scheduler.exe input_example3.txt --verbose
```

## Formato de entrada

Cada linea valida del archivo debe tener el formato:

```text
etiqueta; burst; arrival; queue; priority
```

Ejemplo:

```text
A; 6; 0; 1; 5
B; 9; 0; 1; 4
C; 10; 0; 2; 3
D; 15; 0; 2; 3
E; 8; 0; 3; 2
```

Reglas:

- Se permiten espacios alrededor de `;`.
- Se ignoran lineas vacias.
- Se ignoran lineas que comienzan con `#`.
- `queue` y `priority` son opcionales.
- `burst` debe ser mayor que `0`.
- `arrival`, `queue` y `priority` deben ser mayores o iguales que `0`.

## Algoritmos incluidos

### FCFS

First-Come, First-Served no expropiativo.

### SJF

Shortest Job First no expropiativo.

### STCF / SRTF

Shortest Time to Completion First, version expropiativa basada en tiempo restante.

### Round Robin

Planificacion por turnos con quantum configurable desde linea de comandos.

## Salida esperada

Para cada algoritmo el programa imprime la secuencia de ejecucion. Ejemplo:

```text
FCFS:
A  Desde: 0  Hasta: 6
B  Desde: 6  Hasta: 15

SJF:
A  Desde: 0  Hasta: 6
E  Desde: 6  Hasta: 14

STCF:
A  Desde: 0  Hasta: 2
B  Desde: 2  Hasta: 4
C  Desde: 4  Hasta: 5

Round Robin (Quantum: 4):
A  Desde: 0  Hasta: 4
B  Desde: 4  Hasta: 8
```

Al final de cada bloque se muestran metricas como:

- `Turnaround avg`
- `Waiting avg`

## Archivos de ejemplo

Si tienes estos archivos en el mismo directorio que el ejecutable, puedes probar rapidamente:

- `input_example1.txt`
- `input_example2.txt`
- `input_example3.txt`

## Notas

- `main(int argc, char* argv[])` solo maneja argumentos y dispara la simulacion.
- La lectura del archivo debe estar separada del `main`.
- Cada algoritmo debe mantenerse en una funcion separada.
