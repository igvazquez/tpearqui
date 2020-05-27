#include <expTree.h>
#include <usrlib.h>

#define NULL 0
#define TRUE 1
#define FALSE 0
#define MAX_NODES 30

static int isOperator(char c);

static int getNumber(char *exp, char *buffer);
static void operate(struct Node *node, float *num);
static struct Node *buildExpression(char *exp);
static struct Node *buildExpressionRec(char *exp, int *counter);
static void strToFloat(char *s, float *num);
void ftoa(double n, char *res, int afterpoint);
int intToStr(int x, char str[], int d);
void reverse(char *str, int len);

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

void calculate(char *exp)
{

    struct Node *root = buildExpression(exp);
    if (root == NULL)
    {
        print("La expresion: ");
        print(exp);
        println(" es invalida. Por favor intente de nuevo.");
    }
    else
    {

        float res = 0;
        operate(root, &res);
        char str[NUMBER_LENGTH];
        ftoa(res, str, 4);
        print(exp);
        print(" = ");
        println(str);
    }
}

static struct Node *buildExpression(char *exp)
{
    int counter = 0;
    return buildExpressionRec(exp, &counter);
}

static struct Node *buildExpressionRec(char *exp, int *counter)
{

    struct Node *auxi = NULL;
    if (node_count <= MAX_NODES)
    {
        auxi = &nodes[node_count++];
    }
    else
    {
        print("Expresion is too long");
        return NULL;
    }

    if (exp[*counter] != '\0')
    {

        if (exp[*counter] == '(')
        {
            *counter = *counter + 1;
            struct Node *left = buildExpressionRec(exp, counter);
            if (left == NULL)
            {

                return NULL;
            }
            auxi->left = left;

            if (isOperator(exp[*counter]))
            {

                (auxi->value)[0] = exp[*counter];
                (auxi->value)[1] = '\0';
                *counter = *counter + 1;
            }
            else
            {
                putchar(exp[*counter]);

                return NULL;
            }
            struct Node *right = buildExpressionRec(exp, counter);
            if (right == NULL)
            {

                return NULL;
            }
            auxi->right = right;

            if (exp[*counter] == ')')
            {
                *counter = *counter + 1;
            }
            else
            {

                return NULL;
            }
        }
        else
        {

            if (getNumber(exp + *counter, auxi->value) == FALSE)
            {

                return NULL;
            };

            *counter = *counter + (strlen(auxi->value));

            auxi->left = auxi->right = NULL;
        }
    }
    return auxi;
}

static int getNumber(char *exp, char *buffer)
{

    int decimal = FALSE;
    int ok = TRUE;
    int i = 0;
    char c;
    while (ok == TRUE && i <= NUMBER_LENGTH && (((c = *exp++) >= '0' && c <= '9') || c == '.'))
    {

        if (c == '.' || c == ',')
        {
            if (decimal == 1)
            {
                ok = 0;
            }
            else
            {
                buffer[i++] = c;
                decimal = TRUE;
            }
        }
        else
        {

            buffer[i++] = c;
        }
    }
    if ((c != ')' && isOperator(c) != TRUE) || i > NUMBER_LENGTH)
    {

        ok = FALSE;
    }
    else
    {
        buffer[i] = '\0';
    }

    return ok;
}

static void strToFloat(char *s, float *num)
{

    int decimal = FALSE;
    int multDec = 10;
    while (*s != '\0')
    {

        if (*s == '.')
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

    int ipart = (int)n;

    double fpart = n - (float)ipart;

    int i = intToStr(ipart, res, 0);

    if (afterpoint != 0)
    {
        res[i] = '.'; // add dot

        while (afterpoint > 0)
        {
            fpart = fpart * 10;
            afterpoint--;
        }

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

static void operate(struct Node *node, float *res)
{
    if (node->left == NULL || node->right == NULL)
    {

        strToFloat(node->value, res);
    }
    else
    {
        char op = (node->value)[0];

        float left, right;
        operate(node->left, &left);
        operate(node->right, &right);
        switch (op)
        {
        case '+':
            *res = left + right;
            break;
        case '-':
            *res = left - right;
            break;
        case '*':
            *res = left * right;
            break;
        case '%':
            *res = left / right;
            break;
        }
    }
}
