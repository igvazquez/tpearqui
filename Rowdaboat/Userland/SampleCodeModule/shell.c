//shell.c
#include <shell.h>
#include <usrlib.h>
#include <calculator.h>
#include <commandInterpreter.h>

//constantes para la definicion de arrays
// #define SHELL_MESSAGE "Shell: $>"
// #define USER_INPUT_SIZE 30
// #define MAX_FUNCTIONS 20
// #define MAX_ARGUMENTS_SIZE 5
#define SHELLS 2
// #define OPERATORS 7
// #define VALID_CHARS_CALC 18

#define END_OF_EXECUTION_KEY 27
#define CURSOR_COLOR 0x00FF00

//Vars
//Estructura para el guardado de los modulos. Puntero a la funcion pertinente,
// nombre con el cual se la llama y una breve descripcion de su funcionamiento.
// typedef struct
// {
//     void (*function)(int argcount, char *args[]);
//     char *name;
//     char *description;
// } functionPackage;

// functionPackage functions[MAX_FUNCTIONS];
// //Cantidad de funciones disponibles
// int functionsSize = 0;

int currentScreen = LEFT;
// int shellIndex = 0;
// int calcIndex = 0;
// int pendingInstruction = 0;

// char validChars[VALID_CHARS_CALC] = {'.', '+', '-', '*', '%', '(', ')', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

int cursorTick = 0;

//End
//Protoripos
//Funciones para el cargado del fuctionArray. Carga todos los modulos disponibles.
// static void loadFunctions();
// static void loadFunction(char *string, void (*fn)(), char *desc);

//Funciones utilizadas para la operacion de la shell.
static int readUserInput();
// static void processInstruction(char *userInput);
// static void processCalculation(char *userInput);

//Funciones auxiliares para tener un cursor parpadeante.
static void tickCursor();
static void turnOffCursor();

// //Modulos - Funciones ejecutables desde la shell
// //inforeg: imprimir el valor de todos los registros
// extern void getRegs(int argcount, char *args[]);
// void printRegs(uint64_t *regs); //Llamada por getRegs en assembler.

// //printmem: imprime en pantalla el valor hexadecimal de los primeros 32 bytes
// // a partir de la direccion recibida como argumento
// static void printmem(int argcount, char *args[]);

// //clock: imprime la hora actual en horario GMT-3
// static void printCurrentTime(int argcount, char *args[]);
// static void printTime(enum time id);

// //help: funcion que imprime las funciones disponibles y su respectiva descripcion
// static void help(int argcount, char *args[]);

// //triggerException: funciones demostrativas de las rutinas de atencion de las excepciones
// // 0 y 6 (division por cero y opcode invalido, respectivamente).
// static void triggerException0(int argcount, char *args[]);
// static void triggerException6(int argcount, char *args[]);

//Modulos adicionales
static void startTerminal();
// //ticksElpased: funcion demostrativa de la syscall 0.
// //Imprime los ticks actuales.
// static void ticksElpased(int argcount, char *args[]);

// //printArgs: funcion demostrativa del parseado de argumentos.
// // Imprime todos los argumentos que recibe.
// static void printArgs(int argcount, char *args[]);

void startShell()
{

    clearScreen();

    // char selector = 'b';
    // setCursorPos(12, 3);
    // setSize(4);
    // printf("TPE DE PC \n \n \n \n \n", 0xFF44FF, 0);
    // setSize(2);
    // printf("    legajo, Vazquez     59489, Torrusio     legajo, Monaco \n \n", 0x5CFEE4, 0);

    // printf("                    0)Terminal  1)Calculadora", 0x5CFEE4, 0);
    // setSize(1);
    // while (((selector = getChar()) != '0') && (selector != '1'))
    //     ;
    startTerminal();
}

static void startTerminal()
{
    //Se cargan los modulos

    clearScreen();

    //start calculator
    setScreen(RIGHT);
    initCalc();
    loadCalcScreen();

    //start shell command terminal
    setScreen(LEFT);
    initCommandInt();
    loadCommIntScreen();

    //draw line
    // for (int i = 0; i < verticalPixelCount(); i++)
    // {
    //     drawPixel(horizontalPixelCount() / 2, i, 0x2b66cc);
    //     drawPixel(horizontalPixelCount() / 2 + 1, i, 0x2b66cc);
    //     drawPixel(horizontalPixelCount() / 2 + 2, i, 0x2b66cc);
    // };

    for (int i = 0; i < verticalPixelCount(); i++)
    {
        drawPixel(horizontalPixelCount() / 2, i, 0x2b66cc);
    }

    // if (selector == '1')
    // {
    //     currentShell = CALCULATOR;
    //     setCursorPos(getScreenWidth() / 2 + strlen(SHELL_MESSAGE) + calcIndex + 1, getScreenHeight() - 1);
    // }
    // else if (selector == '0')
    // {
    //     currentShell = SHELL;
    //     setCursorPos(strlen(SHELL_MESSAGE) + shellIndex, getScreenHeight() - 1);
    // }
    // char *userInput[SHELLS];
    // char shellBuffer[USER_INPUT_SIZE];
    // char calcBuffer[USER_INPUT_SIZE];

    // userInput[LEFT] = shellBuffer;
    // userInput[RIGHT] = calcBuffer;

    //Se espera hasta que se reciba un enter y luego, se procesa el texto ingresado.
    //Si coincide con el nombre de una funcion se la ejecuta, sino se vuelve a modo lectura.
    // while (readUserInput(userInput, USER_INPUT_SIZE))
    // {

    //     if (currentScreen == LEFT && pendingInstruction)
    //     {
    //         processInstruction(userInput[LEFT]);
    //         printf(SHELL_MESSAGE, 0x5CFEE4, 0);
    //     }
    //     else if (pendingInstruction)
    //     {

    //         processCalculation(userInput[RIGHT]);
    //         printf(SHELL_MESSAGE, 0x5CFEE4, 0);
    //     }
    //     pendingInstruction = 0;

    //     for (int i = 0; i < verticalPixelCount(); i++)
    //     {
    //         drawPixel(horizontalPixelCount() / 2, i, 0x2b66cc);
    //     }
    // }
    while (readUserInput())
        ;
}

//Funcion encargada de la lectura del texto ingresado por el usuario.
//Se encarga de guardarlo en un buffer para luego ser procesado. Maneja borrado,
// tecla especial para volver al juego y tecla especial para el corte de ejecucion.
static int readUserInput()
{
    char c;
    int currentTimerTick;
    int lastTimerTick = -1;

    while ((c = getChar()))
    {
        //Parpadeo del cursor.
        currentTimerTick = getTicksElapsed();
        if (currentTimerTick != lastTimerTick && currentTimerTick % 10 == 0)
        {
            tickCursor();
            lastTimerTick = currentTimerTick;
        }
        //Procesado de la tecla presionada
        if (c)
        {
            turnOffCursor();

            if (c == END_OF_EXECUTION_KEY)
                return 0;

            if (c == '\t')
            {
                if (currentScreen == LEFT)
                {
                    setScreen(RIGHT);
                    currentScreen = RIGHT;
                    loadCalcScreen();

                    // setCursorPos(getScreenWidth() / 2 + 1, getScreenHeight() - 1);
                    // printf(SHELL_MESSAGE, 0x5CFEE4, 0);
                    // setCursorPos(getScreenWidth() / 2 + strlen(SHELL_MESSAGE) + calcIndex + 1, getScreenHeight() - 1);
                }
                else if (currentScreen == RIGHT)
                {
                    setScreen(LEFT);
                    currentScreen = LEFT;
                    loadCommIntScreen();
                    // setCursorPos(0, getScreenHeight() - 1);
                    // printf(SHELL_MESSAGE, 0x5CFEE4, 0);
                    // setCursorPos(strlen(SHELL_MESSAGE) + shellIndex, getScreenHeight() - 1);
                }
            }
            else //if (c != '\b')
            {
                if (currentScreen == LEFT)
                {
                    // putchar(c);
                    // buffer[LEFT][shellIndex++] = c;
                    processCommandInput(c);
                }
                else if (currentScreen == RIGHT)
                {
                    // for (int i = 0; i < VALID_CHARS_CALC; i++)
                    // {
                    //     if (c == validChars[i])
                    //     {
                    //         putchar(c);
                    //         buffer[RIGHT][calcIndex++] = c;
                    //     }
                    // }
                    processCalcInput(c);
                }
            }
            // else if (currentScreen == LEFT && shellIndex > 0 && c == '\b')
            // {
            //     putchar('\b');
            //     shellIndex--;
            // }
            // else if (currentScreen == RIGHT && calcIndex > 0 && c == '\b')
            // {
            //     putchar('\b');
            //     calcIndex--;
            // }
        }
    }

    turnOffCursor();
    // if (currentScreen == LEFT)
    // {
    //     buffer[currentScreen][shellIndex++] = '\0';
    //     shellIndex = 0;
    // }
    // else
    // {
    //     buffer[currentScreen][calcIndex++] = '\0';
    //     calcIndex = 0;
    // }
    // pendingInstruction = 1;
    // putchar('\n');
    return 1;
}

// static void processCalculation(char *userInput)
// {
//     calculate(userInput);
// }

//Funcion encargada de procesar el texto recibido. Se guardan los argumentos en un array
// y se verifica si el texto ingresado valida con el nombre de una funcion para asi llamarla.
// static void processInstruction(char *instruction)
// {
//     char *arguments[MAX_ARGUMENTS_SIZE];
//     if (*instruction == '\0')
//     {
//         return;
//     }
//     int argCount = strtok(instruction, ' ', arguments, MAX_ARGUMENTS_SIZE);
//     for (int i = 0; i < functionsSize; i++)
//     {
//         if (strcmp(arguments[0], functions[i].name))
//         {
//             functions[i].function(argCount - 1, arguments + 1);
//             return;
//         }
//     }
//     if (*instruction != 0)
//     {
//         printf("Error: ", PINK, 0);
//         printf(instruction, PINK, 0);
//         printf(" not found\n", PINK, 0);
//     }
// }

//Cargado de los modulos
// static void loadFunctions()
// {
//     loadFunction("inforeg", &getRegs, "Prints all the registers \n");
//     loadFunction("ticks", &ticksElpased, "Prints ticks elapsed from start. Arg: -s for seconds elapsed \n");
//     loadFunction("printArgs", &printArgs, "Prints all its arguments\n ");
//     loadFunction("help", &help, "Prints the description of all functions \n");
//     loadFunction("clock", &printCurrentTime, "Prints the current time. Args: -h prints current hours.  -m prints current minutes.  -s prints current seconds.\n");
//     loadFunction("printmem", &printmem, "Makes a 32 Bytes memory dump to screen from the address passed by argument.\nAddress in hexadecimal and 0 is not valid.\n");
//     loadFunction("triggerException0", &triggerException0, "Triggers Exception number 0 \n");
//     loadFunction("triggerException6", &triggerException6, "Triggers Exception number 6 \n");
// }

// static void loadFunction(char *string, void (*fn)(), char *desc)
// {
//     functions[functionsSize].function = fn;
//     functions[functionsSize].name = string;
//     functions[functionsSize].description = desc;
//     functionsSize++;
// }

static void tickCursor()
{
    if (cursorTick)
        putchar('\b');
    else
        putcharf(' ', 0, CURSOR_COLOR);

    cursorTick = !cursorTick;
}

static void turnOffCursor()
{
    if (cursorTick)
        putchar('\b');
    cursorTick = 0;
}

// //Modulos
// static void ticksElpased(int argcount, char *args[])
// {
//     if (strcmp(args[0], "-s"))
//         printint(getTicksElapsed() / 18);
//     else
//         printint(getTicksElapsed());
//     putchar('\n');
// }

// static void printArgs(int argcount, char *args[])
// {
//     for (int i = 0; i < argcount; i++)
//         println(args[i]);
// }

// static void help(int argcount, char *args[])
// {

//     if (argcount >= 1)
//     {
//         for (int i = 0; i < functionsSize; i++)
//         {
//             if (strcmp(functions[i].name, args[0]))
//             {
//                 print("Function ");
//                 println(functions[i].name);
//                 println(functions[i].description);
//                 return;
//             }
//         }
//         print(args[0]);
//         println(" is not a command. Here is a list of all commands:");
//     }

//     for (int i = 0; i < functionsSize; i++)
//     {

//         print(functions[i].name);
//         print(": ");
//         println(functions[i].description);
//     }
// }

// void printRegs(uint64_t *regs)
// {
//     static char *regNames[] = {
//         "RAX: ", "RBX: ", "RCX: ", "RDX: ", "RBP: ", "RDI: ", "RSI: ",
//         "R8: ", "R9: ", "R10: ", "R11: ", "R12: ", "R13: ", "R14: ", "R15: ",
//         "IP: ", "RSP: "};

//     char buffer[20];
//     for (int i = 0; i < 17; i++)
//     {
//         uintToBase(regs[i], buffer, 16);
//         print(regNames[i]);
//         println(buffer);
//     }
// }

// static void printCurrentTime(int argcount, char *args[])
// {
//     if (argcount == 0)
//     {
//         printTime(HOURS);
//         putchar(':');
//         printTime(MINUTES);
//         putchar(':');
//         printTime(SECONDS);
//     }
//     else
//     {
//         if (strcmp(args[0], "-h"))
//             printTime(HOURS);
//         else if (strcmp(args[0], "-m"))
//             printTime(MINUTES);
//         else if (strcmp(args[0], "-s"))
//             printTime(SECONDS);
//         else
//             print("Wrong argument");
//     }
//     putchar('\n');
// }

// static void printTime(enum time id)
// {
//     char buffer[3];
//     int aux;
//     switch (id)
//     {
//     case SECONDS:
//         uintToBase(getTime(SECONDS), buffer, 10);
//         break;
//     case MINUTES:
//         uintToBase(getTime(MINUTES), buffer, 10);
//         break;
//     case HOURS:
//         aux = getTime(HOURS);
//         if (aux < 3)
//             aux = 24 - 3 + aux;
//         else
//             aux -= 3;
//         uintToBase(aux, buffer, 10);
//         break;
//     default:
//         return;
//     }
//     print(buffer);
// }

// static void printmem(int argcount, char *args[])
// {
//     uint64_t num;
//     char buffer[50];
//     if (argcount < 1 || (num = hexstringToInt(args[0])) == 0)
//     {
//         println("invalid argument");
//         return;
//     }
//     uint8_t *address = (uint8_t *)num;

//     for (int i = 0; i < 4; i++)
//     {
//         for (int j = 0; j < 8; j++)
//         {
//             uintToBase(*(address + 8 * i + j), buffer, 16);
//             print(buffer);
//             putchar(' ');
//         }
//         putchar('\n');
//     }
// }

// static void triggerException0(int argcount, char *args[])
// {
//     int a = 0;
//     int b = 4 / a;
//     a = b;
//     return;
// }

// static void triggerException6(int argcount, char *args[])
// {
//     __asm__("ud2"); //https://mudongliang.github.io/x86/html/file_module_x86_id_318.html
// }