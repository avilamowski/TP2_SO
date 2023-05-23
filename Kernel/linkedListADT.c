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

Node *appendElement(LinkedListADT list, void *data) {
	if (list == NULL)
		return NULL;
	Node *newNode = (Node *) allocMemory(sizeof(Node));
	newNode->data = data;
	return appendNode(list, newNode);
}

Node *appendNode(LinkedListADT list, Node *node) {
	if (list == NULL)
		return NULL;
	node->next = NULL;
	if (list->len > 0)
		list->last->next = node;
	else
		list->first = node;
	node->prev = list->last;
	list->last = node;
	list->len++;
	return node;
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
		return NULL;

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
	// free(node);
	return data;
}