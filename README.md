*Este proyecto ha sido creado como parte del currículo de 42 por aitorres.*

---

# Codexion

## Descripción

Codexion es una simulación del problema clásico de los filósofos cenando, adaptada al mundo del desarrollo de software: varios programadores compiten por dongles USB para poder compilar su código.

Cada programador necesita dos dongles (el suyo propio y el del vecino) para compilar. Una vez compilado, los suelta, espera un tiempo de cooldown, y pasa a depurar y refactorizar antes de volver a intentarlo. Si un programador tarda demasiado en conseguir compilar, se quema (*burnout*) y la simulación termina.

El objetivo del proyecto es implementar una solución correcta al problema de concurrencia sin *deadlocks*, sin inanición y con detección precisa del burnout, ofreciendo dos políticas de planificación para la asignación de dongles: **FIFO** y **EDF** (*Earliest Deadline First*).

La simulación es configurable: número de programadores, tiempos de cada fase, compilaciones requeridas, cooldown entre usos del dongle y el planificador a usar.

---

## Instrucciones

### Compilación

```bash
make
```

Esto genera el ejecutable `codexion`. Para limpiar los objetos:

```bash
make clean
```

Para limpiar todo (objetos y ejecutable):

```bash
make fclean
```

Para recompilar desde cero:

```bash
make re
```

Si deseas ejecutar y limpiar:
```bash
make run
```

### Uso

```
./codexion <numb_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

Todos los tiempos están en **milisegundos**. El planificador acepta los valores `fifo` o `edf`.

| Parámetro                   | Descripción                                               |
|-----------------------------|-----------------------------------------------------------|
| `numb_of_coders`          | Número de programadores (y de dongles)                    |
| `time_to_burnout`           | Tiempo máximo sin compilar antes de quemarse              |
| `time_to_compile`           | Duración de la fase de compilación                        |
| `time_to_debug`             | Duración de la fase de depuración                         |
| `time_to_refactor`          | Duración de la fase de refactorización                    |
| `number_of_compiles_required` | Compilaciones que debe completar cada programador        |
| `dongle_cooldown`           | Tiempo de espera del dongle tras ser liberado             |
| `scheduler`                 | Política de planificación: `fifo` o `edf`                 |

### Ejemplos

**Ejecución básica (sin burnout):**
```bash
./codexion 3 800 200 200 200 3 50 fifo
```

**Prueba de burnout (el tiempo de burnout es menor que el ciclo):**
```bash
./codexion 3 100 200 200 200 10 50 fifo
```

**Prueba de cooldown con 2 programadores:**
```bash
./codexion 2 1000 100 100 100 5 500 fifo
```

**Misma prueba con planificador EDF:**
```bash
./codexion 3 800 200 200 200 3 50 edf
```

**Con Valgrind para comprobar fugas de memoria:**
```bash
valgrind ./codexion 3 500 200 100 100 2 100 fifo
```

---

## Blocking cases handled

### Prevención de deadlock (condiciones de Coffman)

El deadlock clásico en el problema de los filósofos ocurre cuando cada hilo toma un recurso y espera indefinidamente por el otro, formando un ciclo. En Codexion se rompe esta situación invirtiendo el orden de adquisición de los dongles según la paridad del ID del programador:

```c
void get_dongle_order_fifo(t_coder *self, t_dongle **first, t_dongle **second)
{
    if (self->id % 2 == 0)
    {
        *first  = self->right_dongle;
        *second = self->left_dongle;
    }
    else
    {
        *first  = self->left_dongle;
        *second = self->right_dongle;
    }
}
```

Los programadores de ID par adquieren primero el dongle derecho y los de ID impar el izquierdo. Esto rompe la simetría y hace imposible que se forme el ciclo de espera circular que define un deadlock.

### Prevención de inanición

Sin un mecanismo de ordenación, un programador podría quedar indefinidamente en espera mientras otros más rápidos se adelantan continuamente. Ambos planificadores abordan esto de forma diferente:

- **FIFO**: la cola de espera de cada dongle es estrictamente por orden de llegada. El primero en encolar es el primero en recibir el dongle cuando se libera. Ningún hilo puede saltarse a otro que ya estaba esperando.

- **EDF**: los hilos con el *deadline* más próximo (es decir, los que están más cerca del burnout) se colocan delante en la cola. Aunque un hilo rápido llegue después, si su deadline es más tardío, espera. Esto garantiza que el programador con más urgencia compile antes de quemarse.

### Gestión del cooldown

Tras liberar los dongles, éstos no están disponibles de inmediato. Se registra el instante en que vuelven a estar disponibles (`available_at_ms`) y, si un hilo llega antes de ese momento, espera mediante `pthread_cond_timedwait` en intervalos cortos de 50 ms para no bloquear indefinidamente ni consumir CPU:

```c
void wait_cooldown_fifo_edf(t_dongle *dongle, t_request *req, t_coder *self)
{
    struct timespec ts;
    long long       now;

    while (!check_burnout(self))
    {
        now = get_current_time_ms();
        if (now >= dongle->available_at_ms)
            break;
        ts.tv_sec  = (now + 50) / 1000;
        ts.tv_nsec = ((now + 50) % 1000) * 1000000;
        pthread_cond_timedwait(&req->cond, &dongle->mutex, &ts);
    }
}
```

Gracias al timedwait, el hilo se desbloquea automáticamente cuando expira el cooldown sin necesidad de que nadie lo despierte explícitamente.

### Detección precisa del burnout

El monitor corre en un hilo separado y comprueba periódicamente si algún programador ha excedido `time_to_burnout` desde su última compilación. La detección es atómica: el primer monitor que detecta el problema activa la bandera `someone_burned` bajo mutex, y si otro hilo llega al mismo tiempo, comprueba que ya estaba activa y no emite un segundo mensaje:

```c
static bool handle_coder_burnout(t_context *ctx, int idx)
{
    pthread_mutex_lock(&ctx->burnout_mutex);
    if (ctx->someone_burned)
    {
        pthread_mutex_unlock(&ctx->burnout_mutex);
        return (true);
    }
    ctx->someone_burned = true;
    pthread_mutex_unlock(&ctx->burnout_mutex);
    log_status(&ctx->coders[idx], "burned out");
    return (true);
}
```

Inmediatamente después, el monitor despierta a todos los hilos bloqueados en colas de dongles mediante `pthread_cond_broadcast` para que salgan limpiamente sin quedarse bloqueados para siempre.

### Serialización del log

Todos los mensajes de estado pasan por `log_status`, que toma el mutex `log_mutex` antes de escribir. Esto garantiza que las líneas de log no se entrelacen entre sí y que el timestamp sea coherente con el estado en el momento de la impresión. Además, una vez detectado el burnout, sólo se permite imprimir el mensaje `"burned out"` del programador afectado; el resto de los estados quedan silenciados para que el output sea determinista.

---

## Thread synchronization mechanisms

### `pthread_mutex_t`: protección de estado compartido

Se usan tres mutexes principales:

- **`dongle->mutex`** (uno por dongle): protege el estado del dongle (`taken`, `available_at_ms`, `wait_queue`). Cualquier hilo que quiera leer o modificar estos campos debe adquirir este mutex primero. Esto evita condiciones de carrera entre hilos que compiten por el mismo recurso.

- **`ctx->burnout_mutex`**: protege la bandera `someone_burned`. Como tanto los hilos de programadores como el monitor pueden escribir sobre ella, el acceso debe ser exclusivo. Leerla sin el mutex podría llevar a que dos hilos detecten el burnout al mismo tiempo y emitan mensajes duplicados.

- **`ctx->log_mutex`**: serializa las escrituras a `stdout`. Sin él, dos `printf` simultáneos podrían solaparse a nivel de bytes en el buffer de salida.

### `pthread_cond_t`: espera eficiente sin *busy-wait*

Cada nodo de la cola de espera (`t_request`) tiene su propia variable de condición `cond`. Cuando un hilo no puede adquirir un dongle, en lugar de iterar en un bucle consumiendo CPU, se suspende sobre su `cond` bajo el mutex del dongle:

```c
void wait_for_grant_or_burnout_fifo(t_dongle *dongle, t_request *req, t_coder *self)
{
    while (!req->granted && !check_burnout(self))
        pthread_cond_wait(&req->cond, &dongle->mutex);
}
```

`pthread_cond_wait` libera atómicamente el mutex y suspende el hilo. Cuando otro hilo llama a `pthread_cond_broadcast` sobre esa condición, el hilo se reactiva, vuelve a adquirir el mutex y comprueba si la concesión fue efectiva o si hay un burnout.

### Protocolo de concesión del dongle

Cuando el poseedor de un dongle lo libera, no simplemente pone `taken = false` y se marcha. Mira si hay alguien en la cola de espera y, si lo hay, le concede directamente el dongle (mantiene `taken = true`) y lo despierta:

```c
static void release_one_dongle(t_dongle *dongle, long next_available)
{
    pthread_mutex_lock(&dongle->mutex);
    dongle->available_at_ms = next_available;
    if (dongle->wait_queue)
    {
        dongle->wait_queue->granted = true;
        pthread_cond_broadcast(&dongle->wait_queue->cond);
    }
    else
        dongle->taken = false;
    pthread_mutex_unlock(&dongle->mutex);
}
```

Esto garantiza que la transición de propietario es atómica: nunca hay un instante en que el dongle esté libre pero nadie lo haya recibido todavía, lo que podría dar lugar a que un hilo recién llegado se adelantase a quien ya llevaba tiempo esperando.

### Comunicación thread-safe entre programadores y monitor

Los programadores y el monitor no comparten datos directamente más allá de `ctx->someone_burned` y el estado de los dongles, ambos protegidos por sus respectivos mutexes. La señal de parada fluye así:

1. El monitor detecta burnout, toma `burnout_mutex`, activa `someone_burned`, lo libera.
2. El monitor llama a `wake_all_dongles`, que recorre todos los dongles y hace `pthread_cond_broadcast` sobre cada nodo de sus colas.
3. Los hilos bloqueados en `pthread_cond_wait` se despiertan, comprueban `check_burnout` (que toma `burnout_mutex` internamente) y, al encontrarlo activo, salen limpiamente.

Esta cadena garantiza que ningún hilo quede suspendido indefinidamente tras el burnout, independientemente de en qué punto del ciclo se encuentre.

### Comportamiento observable de los hilos

Una consideración importante al probar la simulación: los hilos no están numerados por orden de ejecución. El sistema operativo los planifica libremente según la carga del sistema, el estado de la CPU y otros factores. En dos ejecuciones con los mismos parámetros, el orden en que los programadores acceden a los dongles puede variar. Lo que los planificadores FIFO y EDF garantizan es el orden *relativo* dentro de la cola de espera de cada dongle, no el orden global de ejecución entre todos los hilos.

---

## Recursos

### Referencias sobre el problema de los filósofos y concurrencia

- Dijkstra, E. W. (1971). *Hierarchical ordering of sequential processes.* — artículo original donde se formula el problema.
- Tanenbaum, A. S. — *Modern Operating Systems* (cualquier edición). Capítulos sobre sincronización de procesos y problemas clásicos de concurrencia.
- [The Little Book of Semaphores](https://greenteapress.com/wp/semaphore/) — Allen B. Downey. Libro gratuito con variantes del problema y soluciones detalladas.
- [POSIX Threads Programming](https://hpc-tutorials.llnl.gov/posix/) — Blaise Barney, Lawrence Livermore National Laboratory. Referencia práctica completa sobre pthreads.
- Páginas del manual: `man pthread_mutex_lock`, `man pthread_cond_wait`, `man pthread_cond_timedwait`, `man gettimeofday`.

### Sobre planificación EDF

- Liu, C. L., & Layland, J. W. (1973). *Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment.* Journal of the ACM, 20(1). — artículo fundacional del EDF.

### Uso de IA

Durante el desarrollo de este proyecto se utilizó Claude (Anthropic) como herramienta de apoyo en las siguientes áreas:

- **Diseño de la estructura de la cola de espera por dongle (`t_request`, `wait_queue`)**: consultas sobre cómo implementar una lista enlazada por pila de llamadas sin memoria dinámica adicional y cómo insertar nodos FIFO y EDF manteniendo el orden correcto.
- **Implementación de `pthread_cond_wait` y `pthread_cond_timedwait`**: aclaración de la semántica de liberación atómica del mutex al suspender el hilo, y cómo calcular correctamente el `struct timespec` para el timedwait.
- **Depuración de la lógica del cooldown**: discusión sobre cómo gestionar el `available_at_ms` de forma que el hilo que recibe la concesión espere el tiempo correcto antes de usar el dongle.

La lógica de concurrencia, la estructura del proyecto, la detección de burnout, la estrategia anti-deadlock y la implementación de ambos planificadores fueron diseñadas y escritas por el autor.
