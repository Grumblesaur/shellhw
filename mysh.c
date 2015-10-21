#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

// with arguments typically being a few characters each, delimited by spaces
// and constrained to a 512 character buffer, 128 is more than enough slots
// for arguments in our vector
const int MAX_ARGS = 128;
const char error[30] = "An error has occurred.\n";
int errlen = 24;

void errmsg() {
	write(STDERR_FILENO, error, errlen);
	return;
}

// call python interpreter to execute .py files
int runpyfile(char *argv[], int argc, int ampy, int fd) {
	char path[] = "python";
	char * args[MAX_ARGS];
	
	// align arguments with correct argv[] ordering
	args[0] = path;
	int i;
	for (i = 1; i < argc + 1; ++i) {
		args[i] = argv[i - 1];
	}
	args[i] = NULL;
	
	int pid = fork();
	
	if (pid == 0) {
		if (fd != 0) {
			dup2(fd, STDOUT_FILENO);
		}
		if (execvp(args[0], args) == -1) {
			errmsg();
		}
	} else if (!ampy) {
		wait();
	}
	return 1;
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
	
	// call builtin procedure
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
			getcwd(wdbuffer, buffsize);
			write(STDOUT_FILENO, wdbuffer, strlen(wdbuffer));
			write(STDOUT_FILENO, "\n", 1);
			break;
		case 3:
			wait();
			break;
	}	
	return choice;
}

// parse procedure when '>' operator is present
int parse_redirect(char * buffer) {
	char * cptr;
	int argc = 0;
	char * argv[MAX_ARGS];
	int back = 1;
	
	int ampy = hasampy(buffer);
	back += ampy;
	
	// tokenize buffer
	cptr = strtok(buffer, "\t\r\n> ");
	argv[argc] = cptr;
	while (cptr != NULL) {
		cptr = strtok(NULL, "\t\r\n> ");
		argv[++argc] = cptr;
	}
	
	// bail out if we don't have a source AND destination for > operator
	if (argc < 2) {
		errmsg();
		return 0;
	}
	
	argv[argc] = NULL;
	
	// open file for redirection
	int fd = open(argv[argc - back], O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
	argv[argc - back] = NULL;
	
	// make sure this isn't a python file we can just run
	if (ispyfile(argv[0])) {
		return runpyfile(argv, argc, ampy, fd);
	}
	
	int pid = fork();
	if (pid == 0) {
		// redirect child process' standard output to the target file
		dup2(fd, STDOUT_FILENO);	
		if (execvp(argv[0], argv) == -1) {
			errmsg();
		}
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
		return runpyfile(argv, argc, ampy, 0);
	}
	 
	// fork() & exec*() procedure
	int pid = fork();
	if (pid == 0) {
		if(execvp(argv[0], argv) == -1) {
			errmsg();
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
	
	// batch mode
	if (argc > 1) {
		// couldn't figure out how to loop linewise with open() and read();
		// used fopen() and fgets() instead
		FILE * fptr = fopen(argv[1], "r");
		if (fptr == NULL) {
			errmsg();
			exit(EXIT_FAILURE);
		}
		
		// obtain instructions line-by-line
		while (fgets(buffer, bufsize, fptr)) {
			// interpret lines
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
