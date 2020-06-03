//shell.c
#include <shell.h>
#include <usrlib.h>
#include <calculator.h>
#include <commandInterpreter.h>

#define SHELLS 2

#define END_OF_EXECUTION_KEY 27
#define CURSOR_COLOR 0x00FF00

static int currentScreen = LEFT;


//End
//Protoripos

//Funciones utilizadas para la operacion de la shell.
static int readUserInput();

//Modulos adicionales
static void startTerminal();

void startShell()
{
    startTerminal();
}

static void startTerminal()
{
    //start calculator
    setScreen(RIGHT);
    initCalc();
    loadCalcScreen();
    //start shell command terminal
    setScreen(LEFT);
    initCommandInt();
    loadCommIntScreen();

    while (readUserInput())
        ;
}

//Funcion encargada de la lectura del texto ingresado por el usuario.
//Se encarga de guardarlo en un buffer para luego ser procesado. Maneja borrado,
// tecla especial para volver al juego y tecla especial para el corte de ejecucion.
static int readUserInput()
{
    char c;

    while ((c = getChar()))
    {
        //Procesado de la tecla presionada
        if (c)
        {

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


    return 1;
}

