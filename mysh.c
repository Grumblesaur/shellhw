#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "argnode.h"

void swap(char ** x, char ** y) {
	char * temp;
	temp = *x;
	*x = *y;
	*y = temp;
}

int parse(char * buffer) {
	if (strlen(buffer) == 0) {
		return 0;
	}
	// initialize vars for parsing
	char * cptr = buffer;
	char * cptr2;
	int argcount = 0;
	int arglen;
	int listinit = 0;
	
	struct argnode *args = (struct argnode*) malloc(sizeof(struct argnode));
	
	printf("Beginning construction of argument list\n");
	
	while (*cptr != 0) {
		arglen = 0;
		printf("entered outer while-loop\n");
		if (*cptr == ' ' || *cptr == '\t') {
			// ignore superfluous whitespace
			++cptr;
			continue;
			printf("found a whitespace char\n");
		} else if (*cptr != ' ' && *cptr != '\t' && *cptr != 0) {
			// when argument is found, count it
			++argcount;
			cptr2 = cptr;
			printf("found an argument\n");
			// determine its length cptr is its current starting position
			while (*cptr2 != ' ' && *cptr2 != '\t' && cptr2 != 0) {
				// determine its length, exit loop upon space or buffer end
				printf("looking for length of argument\n");
				printf("%s\n", cptr2);
				++arglen;
				++cptr2;
			}
			printf("Found length of argument\n");
			// cptr should be ready to look for the next argument
			// cptr2 will be used to strncpy() to the list node
			swap(&cptr, &cptr2);
			printf("swapped cptrs to prepare for next loop iteration\n");
			if (listinit == 0) {
				// first element of args list
				strncpy(args->data, cptr2, arglen);
			} else {
				// otherwise
				add_node(&args, cptr2, arglen);
			}
		}
	}
	
	printf("Beginning construction of argument vector\n");
	
	// create argument vector for execvp() call
	int argc = arglist_len(&args);
	char argv[argc + 1][512];
	struct argnode * currptr = args;
	int i;
	for (i = 0; i < argc; ++i) {
		strcpy(argv[i], currptr->data);
		currptr = currptr->next;
	}
	argv[argc] = 0;
	destruct_arglist(&args);
	
	int pid = fork();
	if (pid == 0) {
		execv(argv[0], argv);
	}
	// TODO: implement execvp() & fork() logic here
		
	return 1;
}

int ispyfile(char * buffer) {
	if (strstr(buffer, ".py")) return 1;
	return 0;
}

int striswhtspc(char * buffer) {
	while (*buffer != 0) {
		if (*buffer != ' ' && *buffer != '\t' && *buffer != '\n') {
			return 0;
		}
		++buffer;
	}
	return 1;
}

int main(int argc, char * argv[]) {
	if (argc > 1) {
		// TODO:
			// open file
			// execute shell commands in file line-by-line
			// exit();
	} //otherwise,
	
	// init process
	char buffer[512];
	printf("Process initialized.\n");
	// TODO:
		// implement interactive mode loop
		// implement non-exec() commands
	for(;;) {
		printf("mysh> ");
		fgets(buffer, 512, stdin);
		printf("\n");
		
		if (striswhtspc(buffer)) {
			continue;
		}
		
		if (strstr(buffer, "exit")) {
			exit(EXIT_SUCCESS);
		}
		
		parse(buffer);
		
		
	}
	return 0;
}
