run: mysh
	./mysh

mysh: mysh.o argnode.o
	gcc -o mysh mysh.o argnode.o

mysh.o: mysh.c argnode.c
	gcc -c mysh.c argnode.c

clean:
	rm *.o mysh
