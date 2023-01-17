#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "nodes.h"
#include "utils.h"

void Add2List(Node** pphead, unsigned char symb, size_t freq) {
	while (*pphead) {
		if ((*pphead)->freq > freq) {
			break;
		}
		pphead = &((*pphead)->next);
	}
	Node* pnew = (Node*)calloc(1, sizeof(Node));
	if (pnew == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}
	pnew->freq = freq;
	pnew->symb = symb;
	pnew->is_symb = true;
	pnew->next = *pphead;
	*pphead = pnew;
}

void AddNode2List(Node** pphead, Node* node) {
	while (*pphead) {
		if ((*pphead)->freq > node->freq) {
			break;
		}
		pphead = &((*pphead)->next);
	}
	node->next = *pphead;
	*pphead = node;
}

void PrintList(const Node* phead) {
	while (phead) {
		printf("%c (%zu) ", phead->symb, phead->freq);
		phead = phead->next;
	}
	printf("\n");
}

Node* MakeNodeFromNode(Node* left, Node* right) {
	Node* res = (Node*)calloc(1, sizeof(Node));
	if (res == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}
	res->freq = left->freq + right->freq;
	res->is_symb = false;
	res->left = left;
	res->right = right;

	return res;
}

Node* MakeTreeFromList(Node* head) {
	while (head && head->next) {
		Node* left = head;
		Node* right = head->next;
		AddNode2List(&(head->next->next), MakeNodeFromNode(left, right));
		head = head->next->next;
	}
	return head;
}

Node* DelTree(Node* root) {
	if (root) {
		root->left = DelTree(root->left);
		root->right = DelTree(root->right);
		free(root);
	}
	return NULL;
}

void PrintTreeOnSide(const Node* root, size_t level) {
	if (root) {
		PrintTreeOnSide(root->right, level + 1);
		for (size_t i = 0; i < level; ++i) {
			printf("\t");
		}
		printf("%3zu\n", root->freq);
		PrintTreeOnSide(root->left, level + 1);
	}
}

void SimmetricCode(Node* root) {
	if (root == NULL) {
		return;
	}
	if (root->left) {
		strcpy_s(root->left->code, ASCII_SIZE, root->code);
		strcat_s(root->left->code, ASCII_SIZE, "0");
		SimmetricCode(root->left);
	}
	if (root->right)
	{
		strcpy_s(root->right->code, ASCII_SIZE, root->code);
		strcat_s(root->right->code, ASCII_SIZE, "1");
		SimmetricCode(root->right);
	}
}

void TreeCodesToBuffer(const Node* root, size_t level, char** buffer) {
	if (root && buffer) {
		TreeCodesToBuffer(root->right, level + 1, buffer);
		if (root->is_symb) {
			strcpy_s(buffer[root->symb], ASCII_SIZE, root->code);
		}
		TreeCodesToBuffer(root->left, level + 1, buffer);
	}
}

void PrintCodeTreeOnSide(const Node* root, size_t level)
{
	if (root)
	{
		PrintCodeTreeOnSide(root->right, level + 1);
		for (size_t i = 0; i < level; ++i) {
			printf("\t");
		}
		printf("%c (%s)\n", root->symb, root->code);
		PrintCodeTreeOnSide(root->left, level + 1);
	}
}
