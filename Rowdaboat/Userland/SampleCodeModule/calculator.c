#include <calculator.h>
#include <usrlib.h>
#include <stdint.h>
#include <limits.h>

#define NULL 0
#define TRUE 1
#define FALSE 0
#define MAX_NODES 30
#define VALID_CHARS_CALC 19
#define INPUT_MESSAGE "Calc: $>"
#define USER_INPUT_SIZE 119
//Errors
#define INT_OVERFLOW_ERR 2
#define SYNTAX_ERR 3
#define MATH_ERROR 4
#define EXP_LENGTH_ERR 4

//Func Definitions
static int isOperator(char c);
static int getNumber(char *exp, char *buffer);
static int operate(struct Node *node, double *num);
static int buildExpression(struct Node **node, char *exp);
static int buildExpressionRec(struct Node **node, char *exp, int *counter);
static void strToFloat(char *s, double *num);
void ftoa(double n, char *res, int afterpoint);
static void printResult(struct Node *root, char *exp);
int intToStr(int x, char str[], int d);
void reverse(char *str, int len);
static void printError(int code);
static void calculate(char *exp);
static int isValid(char c);
static void enter();
static void erase();
//variables
struct Node nodes[MAX_NODES];
static int node_count = 0;
static char calcBuffer[USER_INPUT_SIZE];
static int calcIndex = 0;
char validChars[VALID_CHARS_CALC] = {'=', '.', '+', '-', '*', '%', '(', ')', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '\b'};
////////////

void initCalc()
{
    printCalcRules();
    printf(INPUT_MESSAGE, 0x5CFEE4, 0);
    loadCalcScreen();
}

void loadCalcScreen()
{

    setCursorPos(getScreenWidth() / 2 + 1 + strlen(INPUT_MESSAGE) + calcIndex, getScreenHeight() - 1);
}

void processCalcInput(char c)
{
    if ((calcIndex < USER_INPUT_SIZE - 2))
    {

        if (isValid(c))
        {
            if (c == '\b')
            {
                erase();
            }
            else
            {
                calcBuffer[calcIndex++] = c;
                putchar(c);
                if (c == '=')
                {
                    enter();
                }
            }
        }
    }
    else
    {
        if (c == '=')
        {
            enter();
        }
        else if (c == '\b')
        {
            erase();
        }
    }
}

static void enter()
{
    calcBuffer[calcIndex] = '\0';
    putchar('\n');
    calcIndex = 0;

    calculate(calcBuffer);
    printf(INPUT_MESSAGE, 0x5CFEE4, 0);
}
static void erase()
{
    if (calcIndex > 0)
    {
        calcIndex--;
        putchar('\b');
    }
}

static int isValid(char c)
{
    for (int i = 0; i < VALID_CHARS_CALC; i++)
    {

        if (c == validChars[i])
        {
            return TRUE;
            break;
        }
    }
    return FALSE;
}

static int isOperator(char c)
{
    if (c == '+' || c == '-' ||
        c == '*' ||
        c == '%')
        return TRUE;
    return FALSE;
}

static int overflowOpCheck(char op, double a, double b)
{
    if (isOperator(op))
    {
        switch (op)
        {
        case '+':

            return a > INT_MAX - b;
            break;
        case '-':
            return a > INT_MAX + b;
            break;
        case '*':

            return a > INT_MAX / b;

            break;
        case '%':
            return a > INT_MAX * b;
            break;
        }
    }
    return -1;
}
void printCalcRules()
{
    println("Welcome to the calculator!");
    println("Rules:");
    println("1) Available operators: +, -, *, %");
    print("2) Maximum number supported: +-");
    printint(INT_MAX);
    putchar('\n');
    println("3) Cover every bynary operation with parenthesis.\nFor example: (1+(2*3))");
    println("4) Press '=' to evaluate the expression");
}

static void calculate(char *exp)
{

    if (*exp == '\n' || *exp == ' ' || *exp == '\0')
    {
        return;
    }

    node_count = 0;
    struct Node *root;
    int result = buildExpression(&root, exp);
    if (result != TRUE)
    {
        printError(result);
    }
    else
    {
        printResult(root, exp);
    }
}

static void printError(int code)
{
    switch (code)
    {

    case SYNTAX_ERR:
        printf("Syntax error\n", PINK, 0);
        break;
    case INT_OVERFLOW_ERR:
        printf("Math error: integer overflow\n", PINK, 0);
        break;

    case EXP_LENGTH_ERR:
        printf("Math error: expression is too long\n", PINK, 0);
        break;
    default:

        break;
    }
}

static void printResult(struct Node *root, char *exp)
{

    double res = 0;
    int result = operate(root, &res);

    if (result != TRUE)
    {

        printError(result);
    }
    else
    {
        char str[INT_LENGTH];
        ftoa(res, str, 4);
        print(exp);
        print(" = ");
        println(str);
    }
}

static int buildExpression(struct Node **node, char *exp)
{
    int counter = 0;
    return buildExpressionRec(node, exp, &counter);
}

static int buildExpressionRec(struct Node **node, char *exp, int *counter)
{

    if (node_count <= MAX_NODES)
    {
        *node = &nodes[node_count++];
    }
    else
    {

        return EXP_LENGTH_ERR;
    }

    if (exp[*counter] != '\0')
    {

        if (exp[*counter] == '(')
        {
            *counter = *counter + 1;
            struct Node *left;
            int leftRes = buildExpressionRec(&left, exp, counter);
            if (leftRes != TRUE)
            {

                return leftRes;
            }
            (*node)->left = left;

            if (isOperator(exp[*counter]))
            {

                ((*node)->value)[0] = exp[*counter];
                ((*node)->value)[1] = '\0';
                *counter = *counter + 1;
            }
            else
            {

                return SYNTAX_ERR;
            }
            struct Node *right;
            int rightRes = buildExpressionRec(&right, exp, counter);
            if (rightRes != TRUE)
            {

                return rightRes;
            }
            (*node)->right = right;

            if (exp[*counter] == ')')
            {
                *counter = *counter + 1;
                return TRUE;
            }
            else
            {

                return SYNTAX_ERR;
            }
        }
        else
        {
            int result = getNumber(exp + *counter, (*node)->value);
            if (result)
            {

                *counter = *counter + (strlen((*node)->value));

                (*node)->left = (*node)->right = NULL;
            }

            return result;
        }
    }
    return TRUE;
}

static int getNumber(char *exp, char *buffer)
{

    int decimal = FALSE;

    int res = TRUE;
    int i = 0;
    char c;
    int limit = INT_LENGTH;
    if (*exp == '-')
    {

        buffer[i++] = '-';
        exp++;
        limit += 1; // Because we add '-'
    }
    while (res == TRUE && (((c = *exp++) >= '0' && c <= '9') || c == '.') && (i < limit))
    {

        if (c == '.')
        {
            if (decimal == TRUE)
            {
                res = FALSE;
            }

            else
            {

                decimal = TRUE;
            }
        }

        buffer[i++] = c;
    }

    if (res == TRUE)
    {

        if ((c != ')' && isOperator(c) != TRUE))
        {

            if (i == limit && c >= '0' && c <= '9')
            {

                res = INT_OVERFLOW_ERR;
            }
            else
            {

                res = SYNTAX_ERR;
            }
        }
        else
        {
            buffer[i] = '\0';
            double num;
            strToFloat(buffer, &num);
            long long aux = (long long)num;
            if (aux < INT_MIN || aux > INT_MAX)
            {

                res = INT_OVERFLOW_ERR;
            }
        }
    }

    return res;
}

static void strToFloat(char *s, double *num)
{

    int decimal = FALSE;
    int negative = FALSE;
    int multDec = 10;
    while (*s != '\0')
    {

        if (*s == '-')
        {
            negative = TRUE;
            s++;
        }
        else if (*s == '.')
        {
            decimal = TRUE;
            s++;
        }
        else if (decimal == FALSE)
        {
            *num = *num * 10;
            *num = *num + *s - '0';
            s++;
        }
        else
        {
            double dec = *s - '0';

            *num = *num + dec / multDec;
            multDec *= 10;
            s++;
        }
    }
    if (negative == TRUE)
    {
        *num = *num * -1;
    }
}

void reverse(char *str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int intToStr(int x, char str[], int d)
{

    int i = 0;
    if (x == 0)
    {
        str[i++] = '0';
        return i;
    }
    while (x)
    {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }
    while (i < d)
        str[i++] = '0';
    reverse(str, i);

    str[i] = '\0';
    return i;
}

void ftoa(double n, char *res, int afterpoint)
{

    if (n < 0)
    {

        n *= -1;
        res[0] = '-';
        res++;
    }
    int ipart = (int)n;

    double fpart = n - (double)ipart;

    int i = intToStr(ipart, res, 0);

    if (afterpoint != 0)
    {
        res[i] = '.'; // add dot

        int cantZeros = 0;
        int k = afterpoint;
        while (k > 0)
        {
            fpart = fpart * 10;
            if ((int)fpart == 0)
            {
                cantZeros++;
            }
            k--;
        }
        if (cantZeros > 0)
        {

            i++;
            int j = cantZeros;
            while (j)
            {
                res[i++] = '0';
                j--;
            }
            if (cantZeros == afterpoint)
            {

                res[i] = '\0';
                return;
            }
        }
        else
        {
            i++;
        }

        if (fpart > 0)
        {
            intToStr((int)fpart, res + i, afterpoint);
        }
    }
}

static int operate(struct Node *node, double *res)
{

    if (node->left == NULL || node->right == NULL)
    {

        strToFloat(node->value, res);
        return TRUE;
    }
    else
    {
        char op = (node->value)[0];

        double left = 0, right = 0;
        operate(node->left, &left);

        operate(node->right, &right);

        switch (op)
        {
        case '+':
            if (!overflowOpCheck(op, left, right))
            {
                *res = left + right;

                return TRUE;
            }
            return INT_OVERFLOW_ERR;

            break;
        case '-':
            if (!overflowOpCheck(op, left, right))
            {
                *res = left - right;
                return TRUE;
            }
            return INT_OVERFLOW_ERR;
            break;
        case '*':
            if (!overflowOpCheck(op, left, right))
            {
                *res = left * right;
                return TRUE;
            }
            return INT_OVERFLOW_ERR;

            break;
        case '%':
            if (!overflowOpCheck(op, left, right))
            {
                *res = left / right;
                return TRUE;
            }
            return INT_OVERFLOW_ERR;

            break;
        default:
            return NULL;
            break;
        }
    }
}
