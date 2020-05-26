#include <screenDriver.h>

unsigned int cursorPosX = 0;
unsigned int cursorPosY = 0;
unsigned int screenWidth;
unsigned int screenHeight;

static int parseSpecialCharacter(char c, unsigned int background);
static void backspace(unsigned int background);
static void enter(unsigned int background);
static void tab();
static void scrollDownOnce(unsigned int background);
static int getXInitialPos(int cursorX);
static int getScreenId(int cursorX);
void putchar(char c)
{
    putcharf(c, DEFAULT_FONT_COLOR, DEFAULT_BACKGROUND_COLOR);
}

void init_screen()
{
    init_VM_Driver();
    screenWidth = horPixelCount() / CHAR_WIDTH;
    screenHeight = verPixelCount() / CHAR_HEIGHT;
}

void setCursorPos(unsigned int x, unsigned int y)
{
    if (x < 0 || x >= screenWidth || y < 0 || y >= screenHeight)
        return;

    cursorPosX = x;
    cursorPosY = y;
}

void putcharf(char c, unsigned int font, unsigned int background)
{
    if (parseSpecialCharacter(c, background))
        return;

    if (cursorPosY >= screenHeight)
        scrollDownOnce(background);

    drawChar(cursorPosX * CHAR_WIDTH, cursorPosY * CHAR_HEIGHT, c, font, background);

    cursorPosX++;
    int screenId = getScreenId(cursorPosX);
    if ((screenId == 0 && cursorPosX >= (screenWidth / 2)) || (screenId == 1 && cursorPosX >= screenWidth))
    {
        cursorPosX = getXInitialPos(cursorPosX);
        cursorPosY++;
    }
}

//Se encarga de interpretar caractere de movimiento de cursor. Realizar un salto de
// linea, borrar y realizar una tabulacion.
static int parseSpecialCharacter(char c, unsigned int background)
{
    switch (c)
    {
    case '\n':
        enter(background);
        break;

    case '\b':
        backspace(background);
        break;

    case '\t':
        break;

    default:
        return 0;
    }
    return 1;
}

void printString(char *string)
{
    while (*string)
    {
        putchar(*string);
        string++;
    }
}

void println(char *string)
{
    printString(string);
    putchar('\n');
}

void printStringf(char *string, unsigned int font, unsigned int background)
{
    while (*string)
    {
        putcharf(*string, font, background);
        string++;
    }
}

int getScreenId(int cursorX)
{
    if (cursorX >= 0 && cursorX < screenWidth / 2)
    {
        return 0; //calculator
    }
    else
    {
        return 1; //command shell
    }
}

int getXInitialPos(int cursorX)
{
    int screenId = getScreenId(cursorPosX);
    return screenId == 0 ? 0 : screenWidth / 2 + 1;
}

//Cuando se esta escribiendo en la ultima linea de la pantalla y se quiere pasar a la proxima
// (fuera de la pantalla) sube todo lo impreso en pantalla y genera una nueva linea
// para continuar escribiendo.

static void scrollDownOnce(unsigned int background)
{
    int screenId = getScreenId(cursorPosX);
    int xLimit = screenId == 0 ? screenWidth / 2 : screenWidth;
    for (int j = 0; j < verPixelCount() - CHAR_HEIGHT; j++)
    {
        for (int i = 0; i < xLimit; i++)
        {
            copyPixel(i, j + CHAR_HEIGHT, i, j);
        }
    }
    int x = getXInitialPos(cursorPosX);
    setCursorPos(x, screenHeight - 1);
    for (int i = x; i < screenWidth / 2; i++)
    {
        putcharf(' ', 0, background);
    }
    setCursorPos(x, screenHeight - 1);
}

void clearScreen()
{
    int x = cursorPosX;
    int y = cursorPosY;

    setCursorPos(getXInitialPos(cursorPosX), 0);
    for (int j = 0; j < screenHeight * (screenWidth / 2); j++)
        putchar(' ');

    setCursorPos(x, y);
}

static void enter(unsigned int background)
{
    if (cursorPosY + 1 >= screenHeight)
        scrollDownOnce(background);
    else
    {

        setCursorPos(getXInitialPos(cursorPosX), cursorPosY + 1);
    }
}

static void backspace(unsigned int background)
{
    int screenId = getScreenId(cursorPosX);
    if ((screenId == 0 && cursorPosX == 0 && cursorPosY == 0) || (screenId == 1 && cursorPosX == screenWidth / 2 && cursorPosY == 0))
    {
        return;
    }

    if ((screenId == 0 && cursorPosX == 0) || (screenId == 1 && cursorPosX == screenWidth / 2))
        drawChar(cursorPosX * CHAR_WIDTH, cursorPosY * CHAR_HEIGHT, ' ', 0, background);
    else
    {
        drawChar((cursorPosX - 1) * CHAR_WIDTH, cursorPosY * CHAR_HEIGHT, ' ', 0, background);
        setCursorPos(cursorPosX - 1, cursorPosY);
    }
}