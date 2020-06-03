//shell.c
#include <shell.h>
#include <usrlib.h>
#include <calculator.h>
#include <commandInterpreter.h>

#define SHELLS 2

#define END_OF_EXECUTION_KEY 27
#define CURSOR_COLOR 0x00FF00

static int currentScreen = LEFT;

static int cursorTick = 0;

//End
//Protoripos

//Funciones utilizadas para la operacion de la shell.
static int readUserInput();

//Funciones auxiliares para tener un cursor parpadeante.
static void tickCursor();
static void turnOffCursor();

//Modulos adicionales
static void startTerminal();

void startShell()
{

    clearScreen();
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

    for (int i = 0; i < verticalPixelCount(); i++)
    {
        drawPixel(horizontalPixelCount() / 2, i, 0x2b66cc);
    }

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
        // //Parpadeo del cursor.
        // currentTimerTick = getTicksElapsed();
        // if (currentTimerTick != lastTimerTick && currentTimerTick % 10 == 0)
        // {
        //     tickCursor();
        //     lastTimerTick = currentTimerTick;
        // }
        //Procesado de la tecla presionada
        if (c)
        {
            // turnOffCursor();

            if (c == END_OF_EXECUTION_KEY)
                return 0;

            if (c == '\t')
            {
                if (currentScreen == LEFT)
                {
                    setScreen(RIGHT);
                    currentScreen = RIGHT;
                    loadCalcScreen();
                }
                else if (currentScreen == RIGHT)
                {
                    setScreen(LEFT);
                    currentScreen = LEFT;
                    loadCommIntScreen();
                }
            }
            else
            {
                if (currentScreen == LEFT)
                {

                    processCommandInput(c);
                }
                else if (currentScreen == RIGHT)
                {

                    processCalcInput(c);
                }
            }
        }
    }

    // turnOffCursor();

    return 1;
}

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
