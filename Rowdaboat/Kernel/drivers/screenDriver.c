#include <screenDriver.h>

#define LEFT 0
#define RIGHT 1

unsigned int cursorPosX = 0;
unsigned int cursorPosY = 0;
unsigned int screenWidth;
unsigned int screenHeight;

unsigned int currentScreen = LEFT;

static int parseSpecialCharacter(char c, unsigned int background);
static void backspace(unsigned int background);
static void enter(unsigned int background);
static void scrollDownOnce(unsigned int background);
static int getXInitialPos(unsigned int screenId);

void putchar(char c)
{
    putcharf(c, DEFAULT_FONT_COLOR, DEFAULT_BACKGROUND_COLOR);
}

void putcharf(char c, unsigned int font, unsigned int background)
{
    if (parseSpecialCharacter(c, background))
        return;

    if (cursorPosY >= screenHeight)

        scrollDownOnce(background);

    int charSize = getCharSize();
    drawChar(cursorPosX * CHAR_WIDTH * charSize, cursorPosY * CHAR_HEIGHT * charSize, c, font, background);

    cursorPosX++;
    if (cursorPosX == (screenWidth / 2) - 1)
    {
        cursorPosX = 0;
        cursorPosY++;
    }
    else if (cursorPosX >= screenWidth)
    {
        cursorPosX = screenWidth / 2 + 1;
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

void init_screen()
{
    init_VM_Driver();

    screenWidth = horPixelCount() / CHAR_WIDTH;
    screenHeight = verPixelCount() / CHAR_HEIGHT;
    for (int i = 0; i < screenHeight * CHAR_HEIGHT; i++)
    {
        drawPixel(screenWidth * CHAR_WIDTH / 2, i, 0x2b66cc);
    }
}

int getScreenHeight()
{
    return screenHeight;
}

int getScreenWidth()
{
    return screenWidth / 2 - 1;
}

void setCursorPos(unsigned int x, unsigned int y)
{
    if (x < 0 || x > screenWidth / 2 || y < 0 || y >= screenHeight)
    {
        return;
    }
    if (currentScreen == LEFT)
    {
        cursorPosX = x;
        cursorPosY = y;
    }
    else if (currentScreen == RIGHT)
    {
        cursorPosX = x + screenWidth / 2 + 1;
        cursorPosY = y;
    }
}

int getXInitialPos(unsigned int screenId)
{
    switch (screenId)
    {
    case LEFT:

        return 0;
        break;
    case RIGHT:
        return screenWidth / 2 + 1;

    default:
        return -1;
        break;
    }
}

int setScreen(unsigned int id)
{
    switch (id)
    {
    case LEFT:

        currentScreen = LEFT;

        break;
    case RIGHT:
        currentScreen = RIGHT;

    default:
        return 0;
        break;
    }
    return 1;
}

//Cuando se esta escribiendo en la ultima linea de la pantalla y se quiere pasar a la proxima
// (fuera de la pantalla) sube todo lo impreso en pantalla y genera una nueva linea
// para continuar escribiendo.

static void scrollDownOnce(unsigned int background)
{
    int xLimit = currentScreen == 0 ? horPixelCount() / 2 - CHAR_WIDTH : horPixelCount();
    int x = getXInitialPos(currentScreen);
    for (int j = 0; j < verPixelCount() - CHAR_HEIGHT; j++)
    {
        for (int i = x * CHAR_WIDTH; i < xLimit; i++)
        {
            copyPixel(i, j + CHAR_HEIGHT, i, j);
        }
    }

    setCursorPos(0, screenHeight - 1);

    for (int i = 0; i < screenWidth / 2 - 1; i++)
    {
        putcharf(' ', 0, background);
    }
    setCursorPos(0, screenHeight - 1);
}

void clearScreen()
{
    int x = cursorPosX;
    int y = cursorPosY;
    setScreen(LEFT);
    setCursorPos(0, 0);
    for (int j = 0; j < screenHeight * (screenWidth / 2); j++)
        putchar(' ');
    setScreen(RIGHT);
    setCursorPos(0, 0);
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

        setCursorPos(0, cursorPosY + 1);
    }
}

static void backspace(unsigned int background)
{

    int charSize = getCharSize();
    if ((currentScreen == 0 && cursorPosX == 0 && cursorPosY == 0) || (currentScreen == 1 && cursorPosX == screenWidth / 2 + 1 && cursorPosY == 0))
    {
        return;
    }

    if ((currentScreen == 0 && cursorPosX == 0) || (currentScreen == 1 && cursorPosX == screenWidth / 2 + 1))
        drawChar(cursorPosX * CHAR_WIDTH * charSize, cursorPosY * CHAR_HEIGHT * charSize, ' ', 0, background);
    else
    {
        drawChar((cursorPosX - 1) * CHAR_WIDTH * charSize, cursorPosY * CHAR_HEIGHT * charSize, ' ', 0, background);
        setCursorPos(cursorPosX - getXInitialPos(currentScreen) - 1, cursorPosY);
    }
}