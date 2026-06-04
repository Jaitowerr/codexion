# Codexion
compite contra el tiempo en este emocionante desafío de concurrencia. Orquesta a múltiples personas que programan compitiendo por un número limitado de dongles USB mediante hilos POSIX, mutexes y planificación inteligente—domina la sincronización de recursos antes de que llegue el agotamiento.


cuando compilemos, que va a recibir el programa:
El programa debe recibir los siguientes argumentos (todos obligatorios):
    number_of_coders 
    time_to_burnout
    time_to_compile 
    time_to_debug 
    time_to_refactor 
    number_of_compiles_required 
    dongle_cooldown 
    scheduler

    Ej: ./codexion 4 500 200 200 200 2 100 fifo
8 argumentos mas el nombre dle rpograma 9 argumentos
Necesitamos un parseo que compruebe:
    - argc == 9
    - minimo 2 programadores number_of_coders sea ≥ 1
    - que los datos sean numericos excepto scheduler, y todos ellos mayores a 0
    - scheduler tiene que ser fifo o edf - compararemos con strcpm()


Necesitamos un mak con:
    - make        # Compila
    - make clean  # Borra objetos
    - make fclean # Borra ejecutable
    - make re     # Limpia y reconstruye


luego como si fuesen objetos debemos crear a los programadores y sin dongles
cada programador tendrá un dongle,
realmente lo haremos como listas, pero el ultimo programador apuntará como next al primer programador, asi tenemos una lista, les guardamos con un id empezando en 1 y creciendo asi podemos dar la vuelta y saber cuando parar en caso de hacer algo
QUE CONTIENe el programodor:
    - ID
    - Veces de compilacion contador
    - ALGO APRA EL TIEMPO DE CUANDO EMPEZÓ A COMPILAR
    - rEFERENCIA A SU DONGLE
    - referencia dongle derecho, el del vecino

Qué contiene el Dongle:
    - UN id que será el mismo que el rpogramador
    - un bool de ocupado o no, o 1 y 0, pero mas facil de entender con bool


para todo esto hay que entender la salida:
    0 1 has taken a dongle
    1 1 has taken a dongle
    1 1 is compiling
    201 1 is debugging
    401 1 is refactorizing
    402 2 has taken a dongle
    403 2 has taken a dongle
    403 2 is compiling
    603 2 is debugging
    803 2 is refactoring
    ...
 que significa cada cosa:
    [timestamp] [id_programador] [mensaje]

Línea por línea
    🔹 0 1 has taken a dongle
        En el momento 0 ms, el programador con ID 1 ha cogido un dongle (el de su izquierda).
    🔹 1 1 has taken a dongle
        En el momento 1 ms, el programador 1 ha cogido el segundo dongle (el de su derecha). ¡Ahora puede compilar!
    🔹 1 1 is compiling
        En el momento 1 ms, el programador 1 empieza a compilar.
    🔹 201 1 is debugging
        Pasan 200 ms → En el momento 201 ms, el programador 1 entra en depuración.
    🔹 401 1 is refactoring
        Pasan otros 200 ms → En el momento 401 ms, el programador 1 entra en refactorización.
    🔹 402 2 has taken a dongle
        En el momento 402 ms, el programador 2 coge su primer dongle.
    🔹 403 2 has taken a dongle
        En el momento 403 ms, el programador 2 coge su segundo dongle. ¡Listo para compilar!
    🔹 403 2 is compiling
        En el momento 403 ms, el programador 2 empieza a compilar.
    🔹 603 2 is debugging
        Pasan 200 ms → En el momento 603 ms, el programador 2 entra en depuración.
    🔹 803 2 is refactoring
        Pasan otros 200 ms → En el momento 803 ms, el programador 2 entra en refactorización.

El tiempo como se calcula:
    Todo se mide en milisegundos desde el inicio del programa.
.
    Programador 1 empieza en 0 ms.
    Compila durante 200 ms → termina en 200 ms.
    Depura 100 ms → termina en 300 ms.
    Refactoriza 100 ms → termina en 400 ms.

Cuando se acaba, por dos cosas:
    - SE QUEMA
    - TODOS HAN COMPILADO   number_of_compiles_required = 5 cada programador

LA QUEMA/LA PURGA
    Cada vez que un programador termina de compilar (o al inicio si nunca ha compilado), se reinicia su contador de tiempo para quemarse.
    Si antes de empezar a compilar otra vez, pasan más de time_to_burnout milisegundos, se quema.
        time_to_burnout = 500 ms
        El programador 1 termina de compilar en el instante 1000 ms.
        Si antes de coger los dongles otra vez, pasan más de 500 ms (es decir, llega al instante 1501 ms), se imprime:
            1501 1 burned out
    Y el programa termina inmediatamente.

PASOS A SEGUIR

Evento	¿Qué implica?
Compilar	Toma 2 dongles, espera time_to_compile ms, los suelta.
Depurar	Después de compilar, espera time_to_debug ms.
Refactorizar	Después de depurar, espera time_to_refactor ms.
Cooldown dongle	Después de soltarlo, no se puede tomar hasta pasados dongle_cooldown ms.
Burnout	Si pasan más de time_to_burnout ms sin empezar a compilar de nuevo, se quema.
Fin del programa	Si todos han compilado number_of_compiles_required veces, o alguien se quema.


frase chula: 
    Imagina un reloj que avanza segundo a segundo. Este es el ciclo de vida de un programador en la mesa


El Ciclo de Vida del Programador (Coder)
[⌛ ESPERANDO ] -> Quiere compilar. Intenta coger 2 dongles (izq y der).
      |
[🏗️ COMPILANDO] -> ¡Tiene los 2 dongles! Bloquea a los demás.
      |            (Dura: time_to_compile)
      |            (Al terminar: suelta los dongles -> empieza cooldown)
      |
[🔍 DEPURANDO ] -> No necesita dongles. Trabaja solo.
      |            (Dura: time_to_debug)
      |
[🔨 REFACTOR  ] -> No necesita dongles. Limpia el código.
      |            (Dura: time_to_refactor)
      |
      +----------> Vuelve a empezar (Intenta compilar de nuevo)

El Dongle y su "Enfriamiento" (Cooldown)
    El dongle es un objeto compartido. Importante: Nadie puede tocar el dongle mientras esté OCUPADO o ENFRIÁNDOSE.

ESTADO: LIBRE 🟢 
      |
   (El programador X lo coge)
      |
ESTADO: OCUPADO 🔴 (Durante time_to_compile)
      |
   (El programador X lo suelta)
      |
ESTADO: ENFRIÁNDOSE ❄️ (Durante dongle_cooldown)
      |
      +----------> Vuelve a ESTADO: LIBRE 🟢



La Regla del Quemado (Burnout)
    El cronómetro se reinicia solo cuando el programador EMPIEZA a compilar.

TEMPORIZADOR DE VIDA: [||||||||||] (Máximo: time_to_burnout)

- Inicio simulation: Cronómetro empieza.
- Si llega a 0: El programador muere -> FIN DEL PROGRAMA.
- Si empieza a compilar: Cronómetro se pone al máximo otra vez.









Funciones Autorizadas para Programación con Hilos POSIX
Te presento información detallada sobre cada función autorizada en tu proyecto con ejemplos prácticos:

Funciones de Gestión de Hilos
pthread_create
Crea un nuevo hilo de ejecución.

c
Copy
int pthread_create(pthread_t *thread, 
                   const pthread_attr_t *attr,
                   void *(*start_routine)(void *),
                   void *arg);
Ejemplo:

c
Copy
#include <pthread.h>
#include <stdio.h>

void* funcion_hilo(void* arg) {
    int id = *(int*)arg;
    printf("Hola desde el hilo %d\n", id);
    return NULL;
}

int main() {
    pthread_t hilo;
    int id = 1;
    
    pthread_create(&hilo, NULL, funcion_hilo, &id);
    pthread_join(hilo, NULL);
    return 0;
}
pthread_join
Espera a que un hilo termine su ejecución.

c
Copy
int pthread_join(pthread_t thread, void **retval);
Ejemplo:

c
Copy
#include <pthread.h>
#include <stdio.h>

void* tarea_hilo(void* arg) {
    printf("Realizando tarea...\n");
    sleep(2); // Simula trabajo
    int* resultado = malloc(sizeof(int));
    *resultado = 42;
    return resultado;
}

int main() {
    pthread_t hilo;
    void* retorno;
    
    pthread_create(&hilo, NULL, tarea_hilo, NULL);
    pthread_join(hilo, &retorno);
    
    printf("Resultado del hilo: %d\n", *(int*)retorno);
    free(retorno);
    return 0;
}
Funciones de Mutex (Exclusión Mutua)
pthread_mutex_init
Inicializa un mutex.

c
Copy
int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr);
pthread_mutex_lock
Bloquea un mutex (espera si ya está bloqueado).

c
Copy
int pthread_mutex_lock(pthread_mutex_t *mutex);
pthread_mutex_unlock
Desbloquea un mutex.

c
Copy
int pthread_mutex_unlock(pthread_mutex_t *mutex);
pthread_mutex_destroy
Destruye un mutex.

c
Copy
int pthread_mutex_destroy(pthread_mutex_t *mutex);
Ejemplo completo de uso de mutex:

c
Copy
#include <pthread.h>
#include <stdio.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int contador = 0;

void* incrementar(void* arg) {
    for(int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex);
        contador++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t hilo1, hilo2;
    
    pthread_create(&hilo1, NULL, incrementar, NULL);
    pthread_create(&hilo2, NULL, incrementar, NULL);
    
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    
    printf("Contador final: %d\n", contador);
    pthread_mutex_destroy(&mutex);
    return 0;
}
Funciones de Variables de Condición
pthread_cond_init
Inicializa una variable de condición.

c
Copy
int pthread_cond_init(pthread_cond_t *cond,
                      const pthread_condattr_t *attr);
pthread_cond_wait
Espera a que se cumpla una condición (libera el mutex mientras espera).

c
Copy
int pthread_cond_wait(pthread_cond_t *cond,
                      pthread_mutex_t *mutex);
pthread_cond_timedwait
Versión con tiempo límite de pthread_cond_wait.

c
Copy
int pthread_cond_timedwait(pthread_cond_t *cond,
                           pthread_mutex_t *mutex,
                           const struct timespec *abstime);
pthread_cond_broadcast
Notifica a todos los hilos esperando en una variable de condición.

c
Copy
int pthread_cond_broadcast(pthread_cond_t *cond);
pthread_cond_destroy
Destruye una variable de condición.

c
Copy
int pthread_cond_destroy(pthread_cond_t *cond);
Ejemplo de productor-consumidor usando variables de condición:

c
Copy
#include <pthread.h>
#include <stdio.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int buffer = 0;
int datos_disponibles = 0;

void* productor(void* arg) {
    for(int i = 0; i < 5; i++) {
        pthread_mutex_lock(&mutex);
        buffer = i+1;
        datos_disponibles = 1;
        printf("Producido: %d\n", i+1);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        usleep(100000); // 0.1 segundos
    }
    return NULL;
}

void* consumidor(void* arg) {
    for(int i = 0; i < 5; i++) {
        pthread_mutex_lock(&mutex);
        while(!datos_disponibles) {
            pthread_cond_wait(&cond, &mutex);
        }
        printf("Consumido: %d\n", buffer);
        datos_disponibles = 0;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t prod, cons;
    
    pthread_create(&prod, NULL, productor, NULL);
    pthread_create(&cons, NULL, consumidor, NULL);
    
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    return 0;
}
Funciones de Tiempo y Entrada/Salida
gettimeofday
Obtiene la fecha y hora actuales.

c
Copy
int gettimeofday(struct timeval *tv, struct timezone *tz);
Ejemplo:

c
Copy
#include <sys/time.h>
#include <stdio.h>

int main() {
    struct timeval tiempo;
    gettimeofday(&tiempo, NULL);
    printf("Segundos: %ld\nMicrosegundos: %ld\n", 
           tiempo.tv_sec, tiempo.tv_usec);
    return 0;
}
usleep
Duerme durante microsegundos.

c
Copy
int usleep(useconds_t usec);
Ejemplo:

c
Copy
#include <unistd.h>
#include <stdio.h>

int main() {
    printf("Inicio\n");
    usleep(2000000); // Duerme 2 segundos
    printf("Pasaron 2 segundos\n");
    return 0;
}
write
Escribe datos a un descriptor de archivo.

c
Copy
ssize_t write(int fd, const void *buf, size_t count);
Ejemplo:

c
Copy
#include <unistd.h>
#include <string.h>

int main() {
    char mensaje[] = "Hola Mundo\n";
    write(STDOUT_FILENO, mensaje, strlen(mensaje));
    return 0;
}
Funciones de Gestión de Memoria
malloc
Asigna memoria dinámica.

c
Copy
void *malloc(size_t size);
free
Libera memoria asignada previamente con malloc.

c
Copy
void free(void *ptr);
Ejemplo:

c
Copy
#include <stdlib.h>
#include <stdio.h>

int main() {
    int* arreglo = (int*)malloc(5 * sizeof(int));
    
    if(arreglo != NULL) {
        for(int i = 0; i < 5; i++) {
            arreglo[i] = i * 2;
            printf("%d ", arreglo[i]);
        }
        printf("\n");
        free(arreglo);
    }
    return 0;
}
Funciones de Impresión y Conversión
printf
Imprime salida formateada a stdout.

c
Copy
int printf(const char *format, ...);
fprintf
Imprime salida formateada a un stream específico.

c
Copy
int fprintf(FILE *stream, const char *format, ...);
Ejemplo:

c
Copy
#include <stdio.h>

int main() {
    printf("Mensaje a pantalla\n");
    fprintf(stderr, "Mensaje a errores\n");
    return 0;
}
strcmp
Compara dos cadenas.

c
Copy
int strcmp(const char *s1, const char *s2);
Ejemplo:

c
Copy
#include <string.h>
#include <stdio.h>

int main() {
    char str1[] = "hola";
    char str2[] = "hola";
    char str3[] = "adios";
    
    if(strcmp(str1, str2) == 0) {
        printf("str1 y str2 son iguales\n");
    }
    
    if(strcmp(str1, str3) < 0) {
        printf("str1 va antes que str3 alfabéticamente\n");
    }
    return 0;
}
strlen
Devuelve la longitud de una cadena.

c
Copy
size_t strlen(const char *s);
Ejemplo:

c
Copy
#include <string.h>
#include <stdio.h>

int main() {
    char texto[] = "Programación";
    printf("Longitud de '%s': %zu\n", texto, strlen(texto));
    return 0;
}
atoi
Convierte una cadena a entero.

c
Copy
int atoi(const char *nptr);
Ejemplo:

c
Copy
#include <stdlib.h>
#include <stdio.h>

int main() {
    char numero_str[] = "12345";
    int numero = atoi(numero_str);
    printf("Número convertido: %d\n", numero + 10);
    return 0;
}
memset
Rellena un bloque de memoria con un valor específico.

c
Copy
void *memset(void *s, int c, size_t n);
Ejemplo:

c
Copy
#include <string.h>
#include <stdio.h>

int main() {
    char buffer[20];
    memset(buffer, 0, sizeof(buffer)); // Rellenar con ceros
    strcpy(buffer, "Texto");
    printf("Buffer: %s\n", buffer);
    return 0;
}
Estas funciones te permitirán implementar programas concurrentes robustos en C usando hilos POSIX. Recuerda siempre inicializar y destruir recursos como mutexes y variables de condición para evitar fugas de memoria o comportamientos indefinidos.















