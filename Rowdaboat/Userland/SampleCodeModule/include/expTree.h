#ifndef __EXPTREE_H__
#define __EXPTREE_H__

struct Node
{
    char value;
    Node *left, *right;
};

Node *newNode(int v);

#endif