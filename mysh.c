#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

// with arguments typically being a few characters each, delimited by spaces
// and constrained to a 512 character buffer, 128 is more than enough slots
// for arguments in our vector
const int MAX_ARGS = 128;
const char error[] = "An error has occurred.\n";

// call python interpreter to execute .py files
void runpyfile(char *argv[], int argc, int ampy) {
	char path[] = "python";
	char * args[MAX_ARGS];
	args[0] = path;
	int i;
	for (i = 1; i < argc + 1; ++i) {
		args[i] = argv[i - 1];
	}
	args[i] = NULL;
	
	int pid = fork();
	if (pid == 0) {
		execvp(path, args);
	} else if (!ampy) {
		wait();
	}
	return;
}

// run as builtin function if possible
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
			break;
		case 2:
			fprintf(stdout, "%s\n", getcwd(wdbuffer, buffsize));
			break;
		case 3:
			wait();
			break;
	}	
	return choice;
}

int parse_redirect(char * buffer) {
	char * cptr;
	int argc = 0;
	char * argv[MAX_ARGS];
	int back = 1;
	
	int ampy = hasampy(buffer);
	back += ampy;
	
	cptr = strtok(buffer, "\t\r\n> ");
	argv[argc] = cptr;
	while (cptr != NULL) {
		cptr = strtok(NULL, "\t\r\n> ");
		argv[++argc] = cptr;
	}
	argv[argc] = NULL;
	
	FILE * fptr = fopen(argv[argc - back], "w");
	int fd = fileno(fptr);
	argv[argc - back] = NULL;
	
	int pid = fork();
	if (pid == 0) {
		dup2(fd, STDOUT_FILENO);
		execvp(argv[0], argv);
	} else if (!ampy) {
		wait();
	}
	
	return 1;
}

// interpret buffer as command
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
	// make sure this isn't a builtin since we're planning to exec() it
	if (builtin(argc, argv) != -1) {
		return 1;
	}
	
	// determine whether we should fork
	ampy = hasampy(argv[argc - 1]);

	// make sure this isn't a python file we can just run
	if (ispyfile(argv[0])) {
		runpyfile(argv, argc, ampy);
		return 1;
	}
	
	// fork() & exec*() procedure
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

// determine whether command is a python file
int ispyfile(char * buffer) {
	return (strstr(buffer, ".py")) ? 1 : 0;
}

// determine whether command should wait()
int hasampy(char * buffer) {
	return (strstr(buffer, "&")) ? 1 : 0;
}

// determine whether command should redirect output
int haswaka(char * buffer) {
	return (strstr(buffer, ">")) ? 1 : 0;
}

// determine whether to skip input
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
	const int bufsize = 512;
	char buffer[bufsize];
	
	fprintf(stdout, "process initialized\n");
	
	// batch mode
	if (argc > 1) {
		fprintf(stdout, "reached batch mode\n");
		FILE * fptr;
		char * line = NULL;
		size_t len = 0;
		ssize_t read;
		
		fptr = fopen(argv[1], "r");
		if (fptr == NULL) {
			fprintf(stderr, error);
			exit(EXIT_FAILURE);
		}
		
		fprintf(stdout, "about to loop through file\n");
		while(read = getline(&line, &len, fptr) != -1) {
			fprintf(stdout, "start loop iteration\n");
			strcpy(buffer, line);
			fprintf(stdout, "%s\n", buffer);
			if (striswhtspc(buffer)) {
				continue;
			}
			if (haswaka(buffer)) {
				parse_redirect(buffer);
			} else {
				parse(buffer);
			}
		}
		return 0;
	}
	
	// interactive mode
	for(;;) {
		fprintf(stdout, "mysh> ");
		fgets(buffer, bufsize, stdin);
		if (striswhtspc(buffer)) {
			continue;
		}
		if (haswaka(buffer)) {
			parse_redirect(buffer);
		} else {
			parse(buffer);
		}
	}
	return 0;
}
