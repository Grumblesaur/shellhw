#include <malloc.h>
#include <stdlib.h>
#include <string.h>

struct argnode {
	char data[64];
	struct argnode * next;
};

int add_node(struct argnode ** list, char * arg, int arglen) {
	struct argnode * currptr;
	
	// create new argnode
	struct argnode * newptr =
		(struct argnode *) malloc(sizeof(struct argnode));

	if (newptr == NULL) {
		return 0;
	}
	
	strncpy(newptr->data, arg, arglen);
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

int arglist_len(struct argnode ** args) {
	struct argnode * currptr = *args;
	int count = 1;
	while (currptr->next != NULL) {
		++count;
		currptr = currptr->next;
	}
	return count;
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
