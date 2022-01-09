# You-re-too-slow

Proyecto semestral para el ramo de Sistemas Operativos, programa en C que sea capaz de simular un juego en base a lo aprendido 
tanto en cátedras como laboratorios, principalmente, en la identificación de sección critica, uso de semáforos, 
tuberías (con y sin nombre), creación de procesos pesados y señales.

En el directorio
**Compilar 
[terminal] $makefile all

**Ejecutar
[terminal-1] $./servidor
[terminal-2] $./cliente

Lineas de codigo relevantes
Servidor 
- Linea 132 a 140 creacion de matriz oculta mediante mmap.
- Linea 153 a 212 creacion de procesos hijos con sus respectivas pipes para la comunicacion con el padre.
- Linea 216 a 368 Implementacion de la logica del juego en el Hijo.
    -Linea 251 a 254 lectura mediante named pipe (FIFO) desde el cliente.
    -Linea 262 a 313 validacion de coordenadas y envio de estas al padre.
    -Linea 316 a 361 Recivo de puntaje desde el padre por unamed pipe y envio al cliente matriz oculta modificada con su puntaje.

- Linea 368 a 430 Implementacion de la logica del juego en el Padre.
    -Linea 376 a 378 Creacion de la matriz real con la logica pedida.
    -Linea 393 a 397 Lectura de coordenadas por unamed pipe desde el hijo.
    -Linea 408 modificacion de matriz oculta alojada en memoria compartida.
    -Linea 411 a 415 comprobacion del puntaje de las coordenadas y envio al hijo mediante unamed pipe.

- Linea 436 a 544 Logica de creacion de la matriz real, para sus valores random.
- Linea 547 a 563 Validacion de coordenadas segun tamaño de la matriz y ocupacion de esta.

Cliente
- Linea 20 a 30 creacion de capturador de señal para el comando de declararse ganador (ctrl + c). 
- Linea 41 a 52 comunicacion por named pipe (FIFO) con el servidor para ingresar datos iniciales (pidid proceso cliente y pidid proceso Padre)
- Linea 65 a 74 Lectura de coordenadas por teclado y escritura de estas mediante named pipe (FIFO) al proceso servidor (Hijo).
- Linea 78 a 84 Lectura de respuesta del servidor hijo y imprecion de estas por pantalla (Matriz oculta y puntaje o respuesta de error).

Este juego esta implementado para solo 1 jugador.
El ingreso de coordenadas es de la sigiente forma: x,y 
El tamaño del tablero para 1 jugador es de 4x4 por lo que las coordenadas son desde el 0 al 3 
ejemplo minimo 0,0 
ejemplo maximo 3,3

Error a tener en cuenta: la conexion FIFO tiene una falla que no sabemos solucionar, el cual consiste en un problema con el buffer el cual no permite avanzar el juego y pasa de manera aleatoria. 