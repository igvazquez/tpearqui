// #ifndef __EXPTREE_H__
// #define __EXPTREE_H__

#define NUMBER_LENGTH 19

struct Node
{
    char value[NUMBER_LENGTH];
    struct Node *left, *right;
};

void calculate(char *exp);

// #endif