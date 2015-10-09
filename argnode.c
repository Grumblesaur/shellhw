#include <malloc.h>
#include <stdlib.h>

struct argnode {
	char data[64];
	struct argnode * next;
};

int add_node(struct argnode ** list, char * arg) {
	struct argnode * currptr;
	
	// create new argnode
	struct argnode * newptr =
		(struct argnode *) malloc(sizeof(struct argnode));

	if (newptr == NULL) {
		return 0;
	}
	
	strcpy(newptr->data, arg);
	newptr->next = NULL;
	
	currptr = *list;
	
	// add to empty arglist
	if (*list == NULL) {
		*list = newptr;
		return 1;
	}
	
	while (currptr->next != NULL) {
		currptr = currptr->next;
	}
	
	currptr->next = newptr;
	return 1;
}

void destruct_arglist(struct argnode ** list) {
	struct argnode * currnode, * nextnode;
	currnode = *list;
	while (currnode->next != NULL) {
		nextnode = currnode->next;
		free(currnode);
		currnode = nextnode;
	}
	return;
}
