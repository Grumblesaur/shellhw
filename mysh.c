#include <stdio.h>
#include <string.h>
#include <unistd.h>

int command(char * buffer) {
	char instruction[512];
	char * cptr;
	if (strlen(instruction) == 0) {
		return 0;
	}
	cptr = strtok(buffer, " \t");
	strcpy(instruction, cptr);
	strcat(instruction, " ");
	while(cptr != NULL) {
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
	
	// TODO:
		// implement interactive mode loop
		// implement non-exec() commands
	
	for(;;) {
		printf("mysh> ");
		fgets(buffer, 512, stdin);
		printf("\n");
		
		if(command(buffer)) {
			continue;
		}
		if(ispyfile(buffer)) {
			system("python %s\n", buffer);
		}
		
	}
	return 0;
}
