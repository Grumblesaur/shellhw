#ifndef ARGNODE_H
#define ARGNODE_H

struct argnode {
	char data[64];
	struct argnode * next;
};

int add_node(struct argnode **, char *, int);
void destruct_arglist(struct argnode **);

#endif
