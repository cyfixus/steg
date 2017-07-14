
all: steg desteg

steg: steg.c
	gcc -Wall -ansi -pedantic -o steg steg.c

desteg: desteg.c
	gcc -Wall -ansi -pedantic -o desteg desteg.c

