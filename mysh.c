#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

// with arguments typically being a few characters each, delimited by spaces
// and constrained to a 512 character buffer, 128 is more than enough slots
// for arguments in our vector
const int MAX_ARGS = 128;
const char error[] = "An error has occurred.\n";

int parse_builtin(char * buffer) {
	const char * builtins[] = {"exit", "cd", "pwd", "wait"};
	char * cptr = buffer;
	int choice = -1;
	
	int buffsize = 1024;
	char wdbuffer[buffsize];
	
	while (*cptr != 0) {
		if(*cptr == '\t' || *cptr == '\n' || *cptr == '\r' || *cptr == ' '){
			++cptr;
		} else {
			int i;
			for (i = 0; i < 4; ++i) {
				if (strncmp(cptr, builtins[i], strlen(builtins[i])) == 0) {
					choice = i;
				}
			}
			break;
		}
	}
		switch (choice) {
			case 0:
				exit(EXIT_SUCCESS);
				return choice;
			case 1:
				// cd logic
				return choice;
			case 2:
				fprintf(stdout, "%s\n", getcwd(wdbuffer, buffsize));
				return choice;
			case 3:
				wait();
				return choice;
		}	
	return 0;
}

int parse(char * buffer) {
	if (strlen(buffer) == 0) {
		return 0;
	}
	// initialize vars for parsing
	char * cptr;
	int argc = 0;
	char * argv[MAX_ARGS];
	int ampy = 0; // should this run in the background?
	
	fprintf(stdout, "Begin tokenizing process\n");
	cptr = strtok(buffer, " \t\r\n");
	argv[argc] = cptr;
	while (cptr != NULL) {
		cptr = strtok(NULL, " \t\r\n");
		argv[++argc] = cptr;
	}
	
	fprintf(stdout, "Terminate argvector with nullptr\n");
	
	int i;
	fprintf(stdout, "argc is %d\n args are:\n", argc);
	for (i = 0; i < argc; ++i) {
		fprintf(stdout, "\t%s\n", argv[i]);
	}
	
	fprintf(stdout, "Check for & instruction\n");	
	if (strcmp(argv[argc - 1], "&") == 0) {
		ampy = 1;
	}
	
	fprintf(stdout, "Start fork() & exec() process\n");	
	int pid = fork();
	if (pid == 0) {
		if(execvp(argv[0], argv) == -1) {
			fprintf(stderr, error);
		}
	} else if (!ampy) {
		wait();
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
		
		parse(buffer);
		
	}
	return 0;
}
