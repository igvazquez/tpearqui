#include <expTree.h>
#include <usrlib.h>

#define NULL 0
#define TRUE 1
#define FALSE 0
#define MAX_NODES 30

static int isOperator(char c);

static int getNumber(char *exp, char *buffer);
static int operate(struct Node *node);
static struct Node *buildExpression(char *exp);
static struct Node *buildExpressionRec(char *exp, int *counter);

struct Node nodes[MAX_NODES];
int node_count = 0;

static int isOperator(char c)
{
    if (c == '+' || c == '-' ||
        c == 'x' ||
        c == '%')
        return TRUE;
    return FALSE;
}

void calculate(char *exp)
{
    struct Node *root = buildExpression(exp);
    if (root == NULL)
    {
        println(exp);
        println("Error: expresion invalida.");
    }
    else
    {
        println(exp);
        println("expresion valida");
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

            if (getNumber(exp, auxi->value) == FALSE)
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
    while (ok == TRUE && i <= NUMBER_LENGTH && (((c = *exp++) >= '0' && c <= '9') || c == '.' || c == ','))
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
    if (c != ')' && isOperator(c) != TRUE)
    {
        println("no es num");
        ok = FALSE;
    }
    else
    {
        buffer[i] = '\0';
    }

    return ok;
}

static int operate(struct Node *node)
{
    if (node->left == NULL || node->right == NULL)
    {
        return node->value;
    }

    char op = *(node->value);
    int res = NULL;
    int left = operate(node->left);
    int right = operate(node->right);
    switch (op)
    {
    case '+':
        res = left + right;
        break;
    case '-':
        res = left - right;
        break;
    case 'x':
        res = left * right;
        break;
    case '%':
        res = left / right;
        break;
    }
    return res;
}
