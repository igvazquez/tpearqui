## Autores
- [Monaco Matias](https://github.com/MatiMonaco)
- [Torrusio Lucia](https://github.com/luciatorrusio)
- [Vazquez Ignacio](https://github.com/igvazquez)

# Manual de usuario:

Entrar a la carpeta Toolchain y ejecutar make all dentro de la misma.
En la carpeta principal, ejecutar make all.
Correr el programa utilizando ./run.sh o sudo ./run.sh si qemu requiere permisos adicionales.

Se abrirá una ventana con el sistema operativo ejecutando el módulo de código del usuario. Desde aquí tendremos el poder de ejecutar los siguientes comandos: inforeg, printArgs, clock, printmem, triggerException0, triggerException6, cpuVendor y cpuTemp. Si presionamos la tecla "tab" accedemos a la calculadora, que realiza expresiones aritmeticas basicas (+, -, * y /) encerrando cada operacion entre parentesis (por ejemplo (2.21*(3.14%2)) ). 

Además, la shell cuenta con el comando ‘help’ que se puede utilizar sin argumentos, y de esta manera la función muestra todos los comandos disponibles con una breve descripción, y la manera en como usarlo. También se puede pedir ayuda sobre una función en particular, ejecutando ‘help <COMMAND>’.

## Kernel:
### Drivers
En primer instancia, se tuvieron que programar los drivers necesarios para poder trabajar durante el tp. Entre estos driver se encuentran el de video, pantalla y teclado. 
El trabajo fue realizado en **modo video** ya que creemos que esta forma de trabajar nos permite tener mas control sobre las acciones visuales que realizamos, es por esto que lo primero que se programó fue el driver de video, en el que se obtienen valores tales como la cantidad de pixeles horizontales y verticales, que se deben cargar llamando a la función init_VM_driver al iniciar el sistema operativo. A su vez, aquí se encuentran las funciones para dibujar un pixel dándole la posición del mismo y su color, y la función para dibujar un  carácter, dependiendo de la función charBitmap del archivo font.c, que se ocupa de devolver el mapa de pixeles que deberá seguir la funcion para dibujar el carácter deseado. Se agrego la funcion copiar pixel que permite dado una posición fuente y una posición destino dibujar sobre la segunda el pixel que se encontraba en la primera.
Una vez terminado el driver de video se procedió a programar el driver de pantalla. Este brinda la funcionalidad de poder considerar a la pantalla como una matriz de caracteres, similar al modo texto y ademas se encarga de manejar los limites para cada pantalla. Para que este funcione correctamente se deberá ejecutar la función init_screen, que se ocupa de delimitar los valores de la matriz en base a la altura y ancho de los caracteres usados y la cantidad de pixeles horizontales y verticales, provistos por el driver de video. Se proveen funciones tales como mover el “cursor”, osea, la posición actual en la matriz, escribir un carácter, ya sea con formato o con los valores default, o del mismo modo imprimir un string. Se tomaron las consideraciones necesarias para el caso de ciertos caracteres especiales tales como el enter o el backspace. A su vez se añade la función de limpiar la pantalla y de subir todos las posiciones una línea, borrando la primera y dejando vacía la última. Consideramos que esta última corresponde en kernel, ya que este modo está pensado para ser usado usualmente por texto y siempre será deseable bajar una línea cuando se alcanza el final de la pantalla.
Para el driver de teclado, se tuvieron que tener ciertas consideraciones. En primer instancia se tuvo que habilitar el PIC para que reciba interrupciones del teclado. Se consideró un teclado en inglés, en el que solo se aceptaban las teclas que posean ASCII dejando afuera a ciertos caracteres como los cursores. Se realizaron las funciones para recibir el codigo del carácter correspondiente usando una tabla de conversión para obtener el ASCII deseado. Los caracteres obtenidos van guardando en un buffer, dándole al usuario la función correspondiente para obtener un carácter del mismo. Al leer las letras se considera si están o no habilitada la tecla de BLOQ MAYUS o si la tecla SHIFT fue presionada y no se ha recibido el breakcode que indique que fue liberada. No usamos los registros correspondientes a estas consideraciones, sino que se fueron analizando los scancode recibidos.
En el caso del driver del reloj, se provee una función que recibe un parámetro, y a partir de este decide devolver la hora, los minutos o los segundos actuales en formato 24 hs.
### Interrupciones
Para trabajar con las interrupciones se utilizó el código provisto por la cátedra, en el que, al recibirse una interrupción, se ocupa de ejecutar la rutina de atención correspondiente (habiendo previamente cargado sus descriptores en la idt) llamando a la función que se ocupará de realizar la acción deseada. Por ejemplo en el caso de recibir una interrupción de teclado, guardar el scancode obtenido. 
Del mismo modo, se programó la función encargada de atender las syscalls, la cual en base al parámetro recibido en el registro RDI, ejecutará la rutina de atención correspondiente, asumiendo que se pasaran por los demás registros los parámetros necesarios para la misma, en el orden que corresponda. Para que dichas llamadas a sistema funcionen se cargó la interrupción 80h de la idt. 
Las syscalls que decidimos ofrecer al usuario desde kernel fueron:
- TimerTick: Devuelve la cantidad de tick desde que se inició el sistema. Para esto fue necesario desenmascarar la IRQ 0.
- sysWrite: Imprime a pantalla donde esté el cursor un string con el color de letra y fondo provistos.
- getChar: Devuelve la tecla tecla más vieja que keyboardDriver tenga almacenada en el buffer. Devuelve 0 si el buffer está vacío.
- setCursorPos: Posiciona el cursor en la posición especificada de la pantalla.
- getScreenWidth(): Devuelve el ancho de la pantalla actual.
- getScreenHeight(): Devuelve el alto de la pantalla actual.
- getTime: Devuelve la hora, minutos o segundos actuales.
- setSize: cambia el tamaño de la fuente.
- setScreen: cambia la pantalla en la que se esta trabajando.
- cpuVendor: devuelve un string con el nombre de nuestro procesador.
- cpuTemp: devuelve la temperatura de nuestro procesador.
### Excepciones
Por último, para las excepciones se siguió el mismo procedimiento. Se tomaron en cuenta la excepción de división por cero y la excepción código de operación inválido. Las mismas se cargaron en la posición 0 y 6 de la idt respectivamente. Al ocurrir una interrupción, se procederá a imprimir en pantalla un mensaje que indique la excepción ocurrida e imprimiendo el estado actual de todos los registros. Se deja un tiempo para que el usuario pueda consultar la información dada y posteriormente se procede a volver al estado inicial del programa principal. Para poder lograr esto, antes de inicializar SampleCodeModule, se guardan las direcciones del IP y del stack, justo antes de ejecutar el programa principal, y antes de realizar el iretq de excepción se modifican los valores del stack y del instruction pointer del stackframe recibido, por dichos valores de back up, asegurando que el programa volverá a comenzar correctamente y no se sobrecarga el stack con las nuevas ejecuciones del programa.

## Userland:
En esta zona del código se cuenta con una librería de usuario donde se brindan funcionalidades de uso general. Se brinda funcionalidad para strings cómo imprimir (con o sin formato), comparar strings, obtener el largo, poder parsear strings y poder convertir un string correspondiente a un número hexadecimal a su valor entero. A su vez se cuenta con funciones tales como la potencia, obtener los valores de alto y ancho de la “matriz” de caracteres considerada en el driver de pantalla y borrar la pantalla, entre otras. Algunas de estas funciones poseen toda su lógica en C y otras utilizan funciones de assembler para poder ejecutar las llamadas a sistema que necesiten.
Nuestro trabajo cuenta con dos modulos principales, un interprete de comandos y una calculadora, encapsulamos ambos modulos dentro de lo que nosotros llamamos "shell", esta es la encargada de leer y gestionar el input del usuario y cambiarnos entre pantallas cuando el usuario ingresa la tecla "TAB". Dependiendo de en qué pantalla este, el shell enviara el input del usuario al método "processInput" que le corresponda al módulo que esta corriendo en dicha pantalla. Cada módulo tiene un buffer propio que se mantiene entre cambios de pantalla, es decir, puedo escribir "hel" en el interprete de comandos, cambiar a la calculadora, realizar una cuenta, volver al interprete y terminar de escribir mi comando poniendo "p" y el buffer tendra el comando "help". Con este sistema, es muy sencillo cambiar el contenido de las pantallas, solo es necesario cambiar los métodos "processInput".

En el caso en que la pantalla actual sea el interprete de comandos, se llamará a la función "processCommandInstruction" con el input recibido por el usuario. Si se apretó la tecla "enter", se procederá a parsear el input y se fijará si el primer argumento coincide con el nombre de alguna de las funciones cargadas, ejecutándola de ser el caso, o enviando un mensaje de error en caso contrario. Finalmente la función dejara el cursor en la última línea, permitiendo al usuario continuar escribiendo.
En el caso de la calculadora, los unicos caracteres validos son numeros, parentesis, "C" o "c" para borrar el input entero, "backspace" para borrar el último carácter y los operadores válidos, en este caso + - * %.
Decidimos que todas las expresiones sean encerradas por paréntesis ya que esto nos facilitaba su interpretacion a la hora de calcular el resultado, debido a que utilizamos un árbol de expresiones. Como nuestro SO es muy basico y no contamos con memoria dinámica, hicimos un vector de nodos del árbol de tamaño constante con un máximo de 30 nodos, tamaño más que aceptable para las expresiones que solemos realizar.
### Funcionalidades disponibles
Se cuenta con la función help que dará información acerca de todas las funcionalidades ejecutables dentro de la terminal. A su vez se la puede invocar con el nombre de alguna función existente como parámetro para recibir sólo la descripción de esta.
La función inforeg, se ocupará de imprimir a pantalla el estado de todos los registros al invocarse esta función teniendo en cuenta el RIP que indica la dirección a la que está apuntando el instruction pointer y el RSP que indica la dirección a la que apunta el puntero de stack. Debido a la implementación utilizada, los valores que se imprimen de rdi y rsi son los que se utilizan para el pasaje de los parámetros al llamar la función, no se puede mantener su valor original. De todas formas, consideramos que no es algo contraproducente ya que, estos registros no reflejan el estado de la shell. Si se quisiese utilizar este programa en un entorno de debugeo, se le podría remover los parámetros que recibe y llamar directamente.
La función printArgs, imprime a pantalla los argumentos con los que fue invocada, dejando un enter entre cada uno. 
La función clock se encarga de imprimir la hora actual, teniendo en cuenta que el procesador envía las horas con un adelanto de 3 en relación a nuestro sistema horario. La hora es impresa en formato HH:MM:SS. Puede invocarse la función con los parámetros -s, -m o -h para obtener únicamente los segundos minutos u horas, respectivamente.
La  función printMem recibe una dirección de memoria como argumento  e imprime las direcciones de las siguientes 32 direcciones de memoria, con su valor en hexa abreviado.
Las funciones triggerException0 y triggerException6 se encargan de simular casos donde se vayan a desencadenar las correspondientes excepciones para poder comprobar el correcto procedimiento de las mismas.
	
## Recursos Externos
- Keyboard scan codes: http://www.cs.umd.edu/~hollings/cs412/s98/project/proj1/scancode
- Invalid opCode (shell - triggerException6): https://mudongliang.github.io/x86/html/file_module_x86_id_318.html
