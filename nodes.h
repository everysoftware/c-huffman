#ifndef NODES_H
#define NODES_H

#include <stdbool.h>
#include <stddef.h>

#define ASCII_SIZE 256

typedef struct Node
{
	unsigned char symb;
	size_t freq;
	unsigned char code[256];
	bool is_symb;
	struct Node* left;
	struct Node* right;
	struct Node *next;
} Node;
void Add2List(Node** pphead, unsigned char symb, size_t freq);
void AddNode2List(Node** pphead, Node* node);
void PrintList(const Node* phead);
Node* MakeNodeFromNode(Node* left, Node* right);
Node* MakeTreeFromList(Node* head);
Node* DelTree(Node* root);
void PrintTreeOnSide(const Node* root, size_t level);
void SimmetricCode(Node* root);
void TreeCodesToBuffer(const Node* root, size_t level, char** buffer);
void PrintCodeTreeOnSide(const Node* root, size_t level);

#endif