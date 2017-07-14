# steg

Steg/Desteg are written in C.
The complimentary programs are meant to hide references to characters in the individual bits of particular bytes within a bitmap. These references are hidden in the least significant bits, to affect the least amount of change in the appearance of the image. 

To compile:
Download desteg.c steg.c and Makefile
In a terminal window located in the folder(hoping you have make and a c compiler), type make
this will create two executables named steg and desteg

To complete a steg operation, you will need a *.BMP and a textfile with text in it.
	./steg Original.BMP Encoded.BMP < textfile

To desteg, 
    ./desteg Encoded.BMP

    the decoded message will appear in the console
