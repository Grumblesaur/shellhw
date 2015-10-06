#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int command(char * buffer) {
	char instruction[512];
	char * cptr;
	if (strlen(buffer) == 0) {
		return 0;
	}

	// commands w/o args
	if (!strstr(buffer, " ") && !strstr(buffer, "\t")) {
		system(buffer);
		return 1;
	}
	
	cptr = strtok(buffer, " \t");
	printf("strtok() and strcat() procedure begun\n");
	strcpy(instruction, cptr);
	strcat(instruction, " ");
	while(cptr != NULL) {
		printf("strcat() loop\n");
		cptr = strtok(buffer, " ");
		strcat(instruction, cptr);
		strcat(instruction, " ");
	}
	system(instruction);
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
		
		if(command(buffer)) {
			continue;
		}
		if(ispyfile(buffer)) {
			system(sprintf("python %s\n", buffer));
		}
		
	}
	return 0;
}
