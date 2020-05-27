#include <expTree.h>
#include <usrlib.h>

#define NULL 0
static bool isOperator(char c);
static double getNumber(char *exp);
static double operate(Node *node);

static bool isOperator(char c)
{
    if (c == '+' || c == '-' ||
        c == 'x' ||
        c == '%')
        return true;
    return false;
}

double calculate(char *exp)
{
    Node *root = buildExpression(exp);
    return operate(root);
}

static Node *buildExpression(char *exp)
{
    Node *auxi = new Node();

    if (*exp != '\0')
    {

        if (*exp == "(")
        {
            exp++;
            auxi->left = buildExpression(exp++);

            if (isOperator(*exp++))
            {
                auxi->value = *exp;
            }
            else
            {
                print("2.Bad expression\n");
            }
            auxi->right = buildExpression(exp++);
            if (*exp == ")")
            {
                exp++;
            }
            else
            {
                print("3.Bad expression\n");
            }
        }
        else
        {
            double number = getNumber(exp);
            auxi->value = number;
            auxi->left = auxi->right = NULL;
        }
        else
        {
            print("4.Bad expression\n");
        }
    }
    return auxi;
}

static double getNumber(char *exp)
{
    double num = 0;
    bool decimal = false;

    int multDec = 10;
    char c;
    while ((c = *exp) >= '0' && c <= '9' || c == '.' || c == ',')
    {

        if (c >= '0' && c <= '9')
        {
            if (!decimal)
            {
                num *= 10;
                num += c - '0';
            }
            else
            {
                int dec = (c - '0') / multDec;
                multDec *= 10;
                num += dec;
            }
        }
        else
        {
            if (c == '.' || c == ',')
            {
                if (decimal)
                {
                    printf("5. Bad expression");
                }
                else
                {
                    decimal = true;
                }
            }
        }
    }
    if (c != ')' && !isOperator(c))
    {
        printf("6. Bad expression");
        return -1;
    }

    return num;
}

static double operate(Node *node)
{
    if (node->left == NULL || node->right == NULL)
    {
        return node->data;
    }

    char op = node->data;
    double res = NULL;
    double left = operate(node->left);
    double right = operate(node->right);
    switch (op)
    {
    case "+":
        res = left + right;
        break;
    case "-":
        res = left - right;
        break;
    case "x":
        res = left * right;
        break;
    case "%":
        res = left / right;
        break;
    }
    return res;
}
