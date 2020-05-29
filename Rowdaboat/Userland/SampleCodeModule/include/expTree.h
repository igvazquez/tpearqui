#ifndef __EXPTREE_H__
#define __EXPTREE_H__

#define INT_LENGTH 10

struct Node
{
    char value[INT_LENGTH + 2];
    struct Node *left, *right;
};

void calculate(char *exp);
void printCalcRules();

#endif