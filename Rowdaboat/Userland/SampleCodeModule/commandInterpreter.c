#include <commandInterpreter.h>
#include <usrlib.h>

#define USER_INPUT_SIZE 55
#define INPUT_MESSAGE "Shell: $>"
#define MAX_ARGUMENTS_SIZE 5
#define MAX_FUNCTIONS 20

static void printCommandIntStart();
static void processInstruction(char *instruction);
static void loadFunctions();
static void loadFunction(char *string, void (*fn)(), char *desc);
//Modulos - Funciones ejecutables desde la shell
//inforeg: imprimir el valor de todos los registros
extern void getRegs(int argcount, char *args[]);
void printRegs(uint64_t *regs); //Llamada por getRegs en assembler.

//printmem: imprime en pantalla el valor hexadecimal de los primeros 32 bytes
// a partir de la direccion recibida como argumento
static void printmem(int argcount, char *args[]);

//clock: imprime la hora actual en horario GMT-3
static void printCurrentTime(int argcount, char *args[]);
static void printTime(enum time id);

//help: funcion que imprime las funciones disponibles y su respectiva descripcion
static void help(int argcount, char *args[]);

//triggerException: funciones demostrativas de las rutinas de atencion de las excepciones
// 0 y 6 (division por cero y opcode invalido, respectivamente).
static void triggerException0(int argcount, char *args[]);
static void triggerException6(int argcount, char *args[]);

//ticksElpased: funcion demostrativa de la syscall 0.
//Imprime los ticks actuales.
static void ticksElpased(int argcount, char *args[]);

//cpuVendor: imprime el modelo de la cpu
extern char *cpuVendor( char *result);
static void cpu();

//printArgs: funcion demostrativa del parseado de argumentos.
// Imprime todos los argumentos que recibe.
static void printArgs(int argcount, char *args[]);
static void enter();
static void erase();
typedef struct
{
    void (*function)(int argcount, char *args[]);
    char *name;
    char *description;
} functionPackage;

functionPackage functions[MAX_FUNCTIONS];
//Cantidad de funciones disponibles
static int functionsSize = 0;

static char buffer[USER_INPUT_SIZE];
static int index = 0;

void initCommandInt()
{
    setCursorPos(0, getScreenHeight() - 1);
    loadFunctions();

    printCommandIntStart();
    printf(INPUT_MESSAGE, 0x5CFEE4, 0);
    loadCommIntScreen();
}

static void printCommandIntStart()
{
    println("Welcome to the shell terminal!");
    println("Please enter 'help' command to see all the available commands.");
}

void loadCommIntScreen()
{
    setCursorPos(0, getScreenHeight() - 1);
    printf(INPUT_MESSAGE, 0x5CFEE4, 0);
    setCursorPos(strlen(INPUT_MESSAGE) + index, getScreenHeight() - 1);
}
void processCommandInput(char c)
{

    if ((index < USER_INPUT_SIZE - 2))
    {

        if (c == '\b')
        {
            erase();
        }
        else
        {

            if (c == '\n')
            {
                enter();
            }
            else
            {
                buffer[index++] = c;
                putchar(c);
            }
        }
    }
    else
    {
        if (c == '\n')
        {
            enter();
        }
        else if (c == '\b')
        {
            erase();
        }
    }
}

static void erase()
{
    if (index > 0)
    {
        index--;
        putchar('\b');
    }
}

static void enter()
{
    buffer[index] = '\0';
    putchar('\n');
    index = 0;

    processInstruction(buffer);
    printf(INPUT_MESSAGE, 0x5CFEE4, 0);
}

static void processInstruction(char *instruction)
{
    char *arguments[MAX_ARGUMENTS_SIZE];
    if (*instruction == '\0')
    {
        return;
    }
    int argCount = strtok(instruction, ' ', arguments, MAX_ARGUMENTS_SIZE);
    for (int i = 0; i < functionsSize; i++)
    {
        if (strcmp(arguments[0], functions[i].name))
        {
            functions[i].function(argCount - 1, arguments + 1);
            return;
        }
    }
    if (*instruction != 0)
    {
        printf("Error:", PINK, 0);
        printf(instruction, PINK, 0);
        printf(" not found\n", PINK, 0);
    }
}

//Cargado de los modulos
static void loadFunctions()
{
    loadFunction("inforeg", &getRegs, "Prints all the registers \n");
    loadFunction("ticks", &ticksElpased, "Prints ticks elapsed from start. Arg: -s for seconds elapsed \n");
    loadFunction("printArgs", &printArgs, "Prints all its arguments\n ");
    loadFunction("help", &help, "Prints the description of all functions \n");
    loadFunction("clock", &printCurrentTime, "Prints the current time. Args: -h prints current hours.  -m prints current minutes.  -s prints current seconds.\n");
    loadFunction("printmem", &printmem, "Makes a 32 Bytes memory dump to screen from the address passed by argument.\nAddress in hexadecimal and 0 is not valid.\n");
    loadFunction("triggerException0", &triggerException0, "Triggers Exception number 0 \n");
    loadFunction("triggerException6", &triggerException6, "Triggers Exception number 6 \n");
    loadFunction("cpuVendor", &cpu, "gets cpu information \n");
}

static void loadFunction(char *string, void (*fn)(), char *desc)
{
    functions[functionsSize].function = fn;
    functions[functionsSize].name = string;
    functions[functionsSize].description = desc;
    functionsSize++;
}

//Modulos
static void ticksElpased(int argcount, char *args[])
{
    if (strcmp(args[0], "-s"))
        printint(getTicksElapsed() / 18);
    else
        printint(getTicksElapsed());
    putchar('\n');
}

static void printArgs(int argcount, char *args[])
{
    for (int i = 0; i < argcount; i++)
        println(args[i]);
}

static void help(int argcount, char *args[])
{

    if (argcount >= 1)
    {
        for (int i = 0; i < functionsSize; i++)
        {
            if (strcmp(functions[i].name, args[0]))
            {
                print("Function ");
                println(functions[i].name);
                println(functions[i].description);
                return;
            }
        }
        print(args[0]);
        println(" is not a command. Here is a list of all commands:");
    }

    for (int i = 0; i < functionsSize; i++)
    {

        print(functions[i].name);
        print(": ");
        println(functions[i].description);
    }
}

void printRegs(uint64_t *regs)
{
    static char *regNames[] = {
        "RAX: ", "RBX: ", "RCX: ", "RDX: ", "RBP: ", "RDI: ", "RSI: ",
        "R8: ", "R9: ", "R10: ", "R11: ", "R12: ", "R13: ", "R14: ", "R15: ",
        "IP: ", "RSP: "};

    char buffer[20];
    for (int i = 0; i < 17; i++)
    {
        uintToBase(regs[i], buffer, 16);
        print(regNames[i]);
        println(buffer);
    }
}

static void printCurrentTime(int argcount, char *args[])
{
    if (argcount == 0)
    {
        printTime(HOURS);
        putchar(':');
        printTime(MINUTES);
        putchar(':');
        printTime(SECONDS);
    }
    else
    {
        if (strcmp(args[0], "-h"))
            printTime(HOURS);
        else if (strcmp(args[0], "-m"))
            printTime(MINUTES);
        else if (strcmp(args[0], "-s"))
            printTime(SECONDS);
        else
            print("Wrong argument");
    }
    putchar('\n');
}

static void printTime(enum time id)
{
    char buffer[3];
    int aux;
    switch (id)
    {
    case SECONDS:
        uintToBase(getTime(SECONDS), buffer, 10);
        break;
    case MINUTES:
        uintToBase(getTime(MINUTES), buffer, 10);
        break;
    case HOURS:
        aux = getTime(HOURS);
        if (aux < 3)
            aux = 24 - 3 + aux;
        else
            aux -= 3;
        uintToBase(aux, buffer, 10);
        break;
    default:
        return;
    }
    print(buffer);
}

static void printmem(int argcount, char *args[])
{
    uint64_t num;
    char buffer[50];
    if (argcount < 1 || (num = hexstringToInt(args[0])) == 0)
    {
        println("invalid argument");
        return;
    }
    uint8_t *address = (uint8_t *)num;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            uintToBase(*(address + 8 * i + j), buffer, 16);

            if (buffer[1] == 0){
                putchar('0');
                print(buffer);
            }else{
                print(buffer);
            }
    
            putchar(' ');
        }
        putchar('\n');
    }
}

static void triggerException0(int argcount, char *args[])
{
    int a = 0;
    int b = 4 / a;
    a = b;
    return;
}

static void triggerException6(int argcount, char *args[])
{
    __asm__("ud2"); //https://mudongliang.github.io/x86/html/file_module_x86_id_318.html
}

static void cpu()
{
    char *result;
    result = 0;
    cpuVendor( result);
    println(result);
}