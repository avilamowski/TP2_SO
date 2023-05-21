#include <defs.h>
#include <linkedListADT.h>
#include <memoryManager.h>

typedef struct LinkedListCDT {
	Node *first;
	Node *last;
	int len;
} LinkedListCDT;

LinkedListADT createLinkedListADT() {
	LinkedListADT list = (LinkedListADT) allocMemory(sizeof(LinkedListCDT));
	list->len = 0;
	list->first = NULL;
	list->last = NULL;
	return list;
}

Node *appendNode(LinkedListADT list, void *data) {
	if (list == NULL)
		return NULL;
	Node *newNode = (Node *) allocMemory(sizeof(Node));
	newNode->next = NULL;
	newNode->data = data;
	if (list->len > 0)
		list->last->next = newNode;
	else
		list->first = newNode;
	newNode->prev = list->last;
	list->last = newNode;
	list->len++;
	return newNode;
}

Node *getFirst(LinkedListADT list) {
	if (list == NULL)
		return NULL;
	return list->first;
}

int isEmpty(LinkedListADT list) {
	if (list == NULL)
		return -1;
	return !list->len;
}

void *removeNode(LinkedListADT list, Node *node) {
	if (list == NULL || node == NULL)
		return;

	if (list->first == node)
		list->first = node->next;
	else
		node->prev->next = node->next;

	if (list->last == node)
		list->last = node->prev;
	else
		node->next->prev = node->prev;

	list->len--;
	void *data = node->data;
	free(node);
	return data;
}