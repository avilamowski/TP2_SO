#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct LinkedListCDT *LinkedListADT;

typedef struct Node {
	void *data;
	struct Node *prev;
	struct Node *next;
} Node;

LinkedListADT createLinkedListADT();
Node *appendElement(LinkedListADT list, void *data);
Node *appendNode(LinkedListADT list, Node *node);
Node *prependNode(LinkedListADT list, Node *node);
void *removeNode(LinkedListADT list, Node *node);
Node *getFirst(LinkedListADT list);
int isEmpty(LinkedListADT list);
void begin(LinkedListADT list);
int hasNext(LinkedListADT list);
void *next(LinkedListADT list);
void freeLinkedListADTDeep(LinkedListADT list);
void freeLinkedListADT(LinkedListADT list);
int getLength(LinkedListADT list);

#endif