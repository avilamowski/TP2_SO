#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct LinkedListCDT *LinkedListADT;

typedef struct Node {
	void *data;
	struct Node *prev;
	struct Node *next;
} Node;

LinkedListADT createLinkedListADT();
Node *appendNode(LinkedListADT list, void *data);
void *removeNode(LinkedListADT list, Node *node);
Node *getFirst(LinkedListADT list);
int isEmpty(LinkedListADT list);
#endif