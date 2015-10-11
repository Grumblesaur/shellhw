#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

const int MAX_ARGS = 64;

int arglen(char ** c) {
	int len = 0;
	while (**c != 0 && **c != '\t' && **c != ' ') {
		++len;
		++(*c);
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
			fprintf(stdout, "Error! Too many arguments!\n");
			return 0;
		}
		if (*cptr == ' ' || *cptr == '\t') {
			// skip whitespace while looking for an argument
			++cptr;
			fprintf(stdout, "skipped a whitespace character\n");
			continue;
		} else if (*cptr == '\0') {
			// end of buffer condition
			fprintf(stdout, "end of command reached\n");
			break;
		} else if (*cptr != '\t' && *cptr != ' ') {
			// argument condition; find length
			fprintf(stdout, "found an argument\n");
			cptr2 = cptr;
			int len = arglen(&cptr2);
			fprintf(stdout, "arglen = %d\n", len);
			// point a unit of the array of pointers toward this argument
			char * arg = malloc((sizeof(char) * (len + 1)));
			strncpy(arg, cptr, len);
			arg[strlen(arg) - 1] = '\0';
			argv[argc++] = arg;
			fprintf(stdout, "argument count is %d\n", argc);
			cptr = cptr2;
		}
	}
	
	int ampy = 0;
	if (strcmp(argv[argc - 1], "&")) {
		ampy = 1;
		*argv[argc - 1] = 0;
	}
	// last argument must delimit argument vector with null pointer
	argv[argc] = NULL;
	
	fprintf(stdout, "execution stage next\n");
		
	// create argument vector for execvp() call
	int pid = fork();
	if (getpid() == 0) {
		if (execvp(argv[0], argv) == -1) {
			fprintf(stdout, "execvp() failed to launch: %s\n", buffer);
		}
	}
	else if (!ampy && getpid() == pid) {
		wait();
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
	if (strlen(buffer) == 0) {
		return 0;
	}
	
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
	fprintf(stdout, "Process initialized.\n");
	// TODO:
		// implement interactive mode loop
		// implement non-exec() commands
	for(;;) {
		fprintf(stdout, "mysh> ");
		fgets(buffer, 512, stdin);
		fprintf(stdout, "\n");
		
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
