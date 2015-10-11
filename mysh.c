#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "argnode.h"

const int MAX_ARGS = 64;

int arglen(char * c) {
	int len = 0;
	while (*c != 0 && *c != '\t' && *c != ' ') {
		++len;
		++c;
	}
	return len;
}

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
	int argc = 0;
	char * argv[MAX_ARGS];

	while (*cptr != 0) {
		if (argc > MAX_ARGS) {
			printf("Error! Too many arguments!\n");
			return 0;
		}
		if (*cptr == ' ' || *cptr == '\t') {
			// skip whitespace while looking for an argument
			++cptr;
			continue;
		} else if (*cptr == '\0') {
			// end of buffer condition
			break;
		} else if (*cptr != '\t' && *cptr != ' ') {
			// argument condition; find length
			cptr2 = cptr;
			int len = arglen(cptr2);
			// point a unit of the array of pointers toward this argument
			char * arg = malloc((sizeof(char) * (len + 1)));
			strncpy(arg, cptr, len);
			arg[strlen(arg) - 1] = '\0';
			argv[argc++] = arg;
		}
	}
	// last argument must delimit argument vector with null pointer
	argv[argc] = NULL;
	
	// create argument vector for execvp() call
	int pid = fork();
	if (pid == 0) {
		execv(argv[0], argv);
	}
	
	// heap cleanup
	int i;
	for (i = 0; i < argc; i++) {
		free(argv[i]);
	}
	
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
