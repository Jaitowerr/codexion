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





valgrind ./codexion 4 500 200 200 200 2 100 fifo



void *coder_executed(void *arg)
{
	t_coder *self;
	
	self = (t_coder *)arg;	//casteo porque recibe void, no sabe que es por lo tanto casteo y debe ir bien, no se si poner un if con la funcion free

	while(self->compile_count < self->config->number_of_compiles_required)
	{
		// printf("COMPILE COUNT: %i\n", self->compile_count);
		printf("PROGRAMADOR ID-%i COMPILE COUNT: %i\n", self->id, self->compile_count);
		
		//REFACTORIZAR
		printf(" - REFACTORIZANDO\n");
		usleep(self->config->time_to_refactor * 1000);		//lo cmabios de milisegundos a microsegundos  y duerme un hilo, significa que deja de consumir cpu ese hilo, El sistema operativo lo pone en pausa, Pasado el tiempo, el hilo se reanuda
		// printf("%d", usleep(self->config->time_to_refactor * 1000));

		//COGER DONGLES
		printf("  - COGER DONGLES\n");

		//COMPILAR
		printf("   - COMPILAR\n");

		//TERMINAR Y SOLTAR DOONGLES
		printf("    - TERMINAR Y SOLTAR DONGLE\n");
	
		self->compile_count++;
	
	}
	
	return NULL;
	
}
primera parte  		//REFACTORIZAR
prueba:
printf("Voy a dormir 1 segundo...\n");
usleep(1000000); // 1.000.000 µs = 1 segundo
printf("¡He despertado!\n");

SEGUNDA PARTE  //COGER DONGLES
El programador necesita coger sus dos dongles (izquierdo y derecho)
¿Qué pasa si otro programador ya tiene uno de esos dongles?

Aquí es donde entra el mutex.
RouteLLM
Routing to Claude Sonnet 4.6
¡Exacto! Aquí es donde entra la parte más importante del proyecto. Vamos paso a paso.

✅ ¿Qué debe hacer el programador aquí?
El programador necesita coger sus dos dongles (izquierdo y derecho) para poder compilar.

Pero hay un problema:

¿Qué pasa si otro programador ya tiene uno de esos dongles?

Aquí es donde entra el mutex.

🧠 ¿Qué es un mutex? (Explicación para tontos)
Imagina que tienes una llave de un baño.

Si alguien está dentro, la llave está cogida y debes esperar.
Cuando sale, devuelve la llave y tú puedes entrar.
Un mutex funciona igual:

pthread_mutex_lock → Coger la llave (si alguien la tiene, esperas)
pthread_mutex_unlock → Devolver la llave
por eso hacemos
pthread_mutex_lock(&self->left_dongle->mutex);   // Espera si está ocupado
pthread_mutex_lock(&self->right_dongle->mutex);  // Espera si está ocupado


vale, ahora incrementamos con el COOLDOWN
despues de usar un dongle, hay que esperar un tiempo de enfriamiento
como lo sincronizamos
    1. Programador suelta el dongle
    2. Guardamos en available_at = ahora + dongle_cooldown
    3. Cuando otro programador intenta cogerlo...
    4. Comprueba si ahora >= available_at
    5. Si no → espera
    6. Si sí → lo coge

















