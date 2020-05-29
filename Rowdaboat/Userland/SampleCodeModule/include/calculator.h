#ifndef __CALCULATOR_H__
#define __CALCULATOR_H__

#define INT_LENGTH 10

struct Node
{
    char value[INT_LENGTH + 2];
    struct Node *left, *right;
};

void printCalcRules();
void processCalcInput(char c);
void loadCalcScreen();
void initCalc();

#endif