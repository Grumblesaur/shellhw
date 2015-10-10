#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "argnode.h"

void swap(char * x, char * y) {
	char * temp;
	temp = x;
	x = y;
	y = temp;
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
	
	while (*cptr != 0) {
		arglen = 0;
		if (*cptr == ' ' || *cptr == '\t') {
			// ignore superfluous whitespace
			++cptr;
			continue;
		} else if (*cptr != ' ' && *cptr != '\t') {
			// when argument is found, count it
			++argcount;
			cptr2 = cptr;
			// determine its length cptr is its current starting position
			while (*cptr2 != ' ' && *cptr != '\t' && cptr != 0) {
				// determine its length, exit loop upon space or buffer end
				++arglen;
				++cptr2;
			}
			// cptr should be ready to look for the next argument
			// cptr2 will be used to strncpy() to the list node
			swap(cptr, cptr2);
			if (listinit == 0) {
				// first element of args list
				strncpy(args->data, cptr2, arglen);
			} else {
				// otherwise
				add_node(&args, cptr2, arglen);
			}
			
		}
		
	}
		
	return 1;
}

int ispyfile(char * buffer) {
	if (strstr(buffer, ".py")) return 1;
	return 0;
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
		
		if (strstr(buffer, "exit")) {
			exit(EXIT_SUCCESS);
		}
		
		if(parse(buffer)) {
			continue;
		}
		if(ispyfile(buffer)) {
			// system(sprintf("python %s\n", buffer));
		}
		
	}
	return 0;
}
