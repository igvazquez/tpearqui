#ifndef __EXPTREE_H__
#define __EXPTREE_H__

#define NUMBER_LENGTH 30

struct Node
{
    char value[NUMBER_LENGTH];
    struct Node *left, *right;
} typedef Node;

void calculate(char *exp)

#endif