#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

// with arguments typically being a few characters each, delimited by spaces
// and constrained to a 512 character buffer, 128 is more than enough slots
// for arguments in our vector
const int MAX_ARGS = 128;
const char error[] = "An error has occurred.\n";

int builtin(int argc, char ** argv) {
	// initialize vars for command-determining logic
	const char * builtins[] = {"exit", "cd", "pwd", "wait"};
	int choice = -1;
	int i;
	
	// initialize vars for getcwd()
	int buffsize = 512;
	char wdbuffer[buffsize];	
	
	// determine which builtin we're calling
	for (i = 0; i < 4; ++i) {
		if (strcmp(argv[0], builtins[i]) == 0) {
			choice = i;
			break;
		}
	}
	
	// call builting procedure
	switch (choice) {
		case 0:
			exit(EXIT_SUCCESS);
		case 1:
			// default to user's home folder if no path is supplied
			if (argv[1] == NULL) {
				chdir(getenv("HOME"));
			} else {
				chdir(argv[1]);
			}
			return choice;
		case 2:
			fprintf(stdout, "%s\n", getcwd(wdbuffer, buffsize));
			return choice;
		case 3:
			wait();
			return choice;
	}	
	return choice;
}

int parse(char * buffer) {
	// can't execute no commands
	if (strlen(buffer) == 0) {
		return 0;
	}

	// initialize vars for parsing
	char * cptr;
	int argc = 0;
	char * argv[MAX_ARGS];
	int ampy = 0; // should this run in the background?

	// tokenize the string in-place; count the number of arguments	
	cptr = strtok(buffer, " \t\r\n");
	argv[argc] = cptr;
	while (cptr != NULL) {
		cptr = strtok(NULL, " \t\r\n");
		argv[++argc] = cptr;
	}
	
	if (builtin(argc, argv) != -1) {
		return 1;
	}
	
	if (strcmp(argv[argc - 1], "&") == 0) {
		ampy = 1;
	}
	
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
	return (strstr(buffer, ".py")) ? 1 : 0;
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
	
	// init
	char buffer[512];
	fprintf(stdout, "Process initialized.\n");
	// TODO:
		// implement interactive mode loop
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
