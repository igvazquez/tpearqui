#include <expTree.h>
#include <usrlib.h>
#include <stdint.h>
#include <limits.h>

#define NULL 0
#define TRUE 1
#define FALSE 0
#define MAX_NODES 30
//Errors
#define INT_OVERFLOW_ERR 2
#define SYNTAX_ERR 3
#define MATH_ERROR 4
#define EXP_LENGTH_ERR 4

//

static int isOperator(char c);

static int getNumber(char *exp, char *buffer);
static int operate(struct Node *node, float *num);
static int buildExpression(struct Node **node, char *exp);
static int buildExpressionRec(struct Node **node, char *exp, int *counter);
static void strToFloat(char *s, float *num);
void ftoa(float n, char *res, int afterpoint);
static void printResult(struct Node *root, char *exp);
int intToStr(int x, char str[], int d);
void reverse(char *str, int len);

static void printError(int code);

struct Node nodes[MAX_NODES];
int node_count = 0;

static int isOperator(char c)
{
    if (c == '+' || c == '-' ||
        c == '*' ||
        c == '%')
        return TRUE;
    return FALSE;
}

static int overflowOpCheck(char op, float a, float b)
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

void calculate(char *exp)
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
    printResult(root, exp);
}

static void printError(int code)
{
    switch (code)
    {

    case SYNTAX_ERR:
        println("Syntax error");
        break;
    case INT_OVERFLOW_ERR:
        println("Math error: integer overflow");
        break;

    case EXP_LENGTH_ERR:
        println("Math error: expression is too long");
        break;
    default:

        break;
    }
}

static void printResult(struct Node *root, char *exp)
{

    println("PRINT RESULT");
    float res = 0;
    int result = operate(root, &res);
    if (result != TRUE)
    {
        println("overflow");
        printError(result);
    }
    else
    {
        char str[20];
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
                print("Operator: ");
                putchar(exp[*counter]);
                putchar('\n');
                ((*node)->value)[0] = exp[*counter];
                ((*node)->value)[1] = '\0';
                *counter = *counter + 1;
            }
            else
            {

                println("ERROR OPERATOR");
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
                println("ERROR )");
                return SYNTAX_ERR;
            }
        }
        else
        {
            int result = getNumber(exp + *counter, (*node)->value);
            if (result)
            {

                print("value: ");
                println((*node)->value);
                *counter = *counter + (strlen((*node)->value));
                print("Counter: ");
                printint(*counter);
                putchar('\n');

                (*node)->left = (*node)->right = NULL;
            }

            return result;
        }
    }
    return TRUE;
}

static int getNumber(char *exp, char *buffer)
{

    print("Number: ");
    putchar(*exp);
    putchar('\n');
    int decimal = FALSE;
    int negative = FALSE;
    int res = TRUE;
    int i = 0;
    char c;
    if (*exp == '-')
    {
        negative = TRUE;
        buffer[i++] = '-';
        exp++;
    }
    while (res == TRUE && i <= NUMBER_LENGTH && (((c = *exp++) >= '0' && c <= '9') || c == '.'))
    {
        if ((negative == TRUE && i > 10) || (negative == FALSE && i > 9))
        {
            return INT_OVERFLOW_ERR;
        }

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
    if ((c != ')' && isOperator(c) != TRUE) || i > NUMBER_LENGTH)
    {

        res = FALSE;
    }
    else
    {
        buffer[i] = '\0';
    }

    if (res)
    {
    }

    return res;
}

static void strToFloat(char *s, float *num)
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
            float dec = *s - '0';

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

void ftoa(float n, char *res, int afterpoint)
{

    if (n < 0)
    {

        n *= -1;
        res[0] = '-';
        res++;
    }
    int ipart = (int)n;

    float fpart = n - (float)ipart;

    int i = intToStr(ipart, res, 0);
    print("i: ");
    printint(i);
    putchar('\n');

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
            print("CantZeros: ");
            printint(cantZeros);
            putchar('\n');
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
        print("fpart: ");
        printint((int)fpart);
        putchar('\n');
        if (fpart > 0)
        {
            intToStr((int)fpart, res + i, afterpoint);
        }
    }
}

static int operate(struct Node *node, float *res)
{
    print("OPERO: ");
    println(node->value);
    if (node->left == NULL || node->right == NULL)
    {
        println("es hoja");
        strToFloat(node->value, res);
        println("strToFloat");
    }
    else
    {
        char op = (node->value)[0];
        print("op: ");
        putchar(op);
        putchar('\n');
        float left = 0, right = 0;
        operate(node->left, &left);
        print("LEFT: ");
        printint((int)left);
        putchar('\n');
        operate(node->right, &right);
        print("RIGHT: ");
        printint((int)right);
        putchar('\n');
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
        }
    }
}
