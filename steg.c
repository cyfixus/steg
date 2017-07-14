/******************
 * cyfixus        *
 * NOV-14-2016    *
*******************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**********************************************************************/
/* Take 4 bytes from an unsigned char array and assemble them into an */
/* int where the first element of the array is the least significant  */
/* byte of the int.                                                   */
/**********************************************************************/
int getIntFromArray(unsigned char bytes[])
{
  int n =
    bytes[0] |
    bytes[1] << 8 |
    bytes[2] << 16 |
    bytes[3] >> 24;
  return n;
}

/***************************************************
 * steg takes the input file, output file, and the header
 * from the header, the image size is obtained to control
 * the loop. 
 * int i and shift are iterators, i iterates through the
 * entire image. shift iterates from 0 to 3, to track if
 * output has compiled completely from char data (stdin)
 * when 3 is reached, shift = 0 and a new char is gathered
 * mask is used to make a mask of data according to the
 * position along data as denoted by shift, every iteration
 * mask is or'd with input to provide an output, <=3 bit 
 * change in byte for each byte beyond the header. */
void steg(FILE *in, FILE *out, unsigned char *header)
{
  int i, shift;
  char mask;
  char data;
  char input;
  char output;
  int pixelWidth;
  int pixelHeight;
  int rowSize;
  int rowPadding;
  int pixelDataSize;
  int fileSize;
  int charOut;

  fileSize = getIntFromArray(&header[2]);
  pixelWidth = getIntFromArray(&header[18]);
  pixelHeight = getIntFromArray(&header[22]);
  /* compute row padding */
  rowSize = pixelWidth*3;
  rowPadding = (4 - (rowSize % 4)) % 4;
  pixelDataSize = rowSize*pixelHeight;
  rowSize += rowPadding;

  shift = 0;

  fseek(in, 54, SEEK_SET);
  fseek(out, 54, SEEK_SET);
    for(i = 0; i < pixelDataSize + rowSize; ++i)
    {
      /*input is gathered verbatim then right shifted 2 and left shifted 2
       *to clear the last two bits for manipulation*/
      input = getc(in);
      input >>= 2;
      input <<= 2;

      switch(shift)
      {
        /* 0 new char from stdin, extract first two bits, shift to two lsb*/
        case 0:
          data = getc(stdin);
          if(data == EOF)
          {
            data = 0;
          }
          mask = data & 192;
          mask >>= 6;
          break;
        /* 1 extract bits 32 and 16, shift to two lsb*/
        case 1:
          mask = data & 48;
          mask >>= 4;
          break;
        /* 2 extract bits 8 and 4, shift to two lsb*/
        case 2:
          mask = data & 12;
          mask >>= 2;
          break;
        /* 3 extract bits 2 and 1, final case, reset stdin char loop*/
        case 3:
          mask = data & 3;
          break;
      }

      output = mask | input;
      putc(output, out);

      if(shift++ == 3)
      {
        shift = 0;
      }
      charOut++;
    }
    if(!feof(in))
    {
      while(charOut < fileSize)
      {
        output = getc(in);
        if(feof(in))
        {
          break;
        }
        putc(output, out);
        charOut++;
      }

    }
}

/**********************************************
 isBMP takes the pointer to header and checks if the 
 first two chars are B M then if the header is the
 correct size based on the data, and then if the file
 is set for 24 bits per pixel*/
int isBMP(unsigned char *header)
{
  if(header[0] != 'B' || header[1] != 'M')
  {
    printf("not a BMP\n");
    return 0;
  }
  if(getIntFromArray(&header[10]) != 54)
  {
    printf("not a valid header size\n");
    return 0;
  }
  if(!(header[28] == 24 && header[29] == 0))
  {
    printf("not valid bits per pixel\n");
    return 0;
  }
  return 1;
}


/******************************************************************************
 * Main takes two arg. fileIn, fileOut. message text will be read from stdin  *
 * in/out headers match (pass the char** around?) to maintain image size      *
 * each char of message hidden in lowest two bits of four bytes pixel data    *
 * RGB bytes and padding bytes. 
 * create a null sequence to denote EOM 0 byte... copy remaining pixel data  */
int main(int argc, char *argv[]){
  FILE *in;
  FILE *out;
  char *fileIn;
  char *fileOut;

  unsigned char header[54];

  fileIn = argv[1];
  fileOut = argv[2];

  if(argc != 3)
  {
    printf("wrong # args\n");
    return 1;
  }
  
  in = fopen(fileIn, "rb");
  out = fopen(fileOut, "wb");

  fread(header, 1, 54, in);/*read file header into header array */

  if(isBMP(header))
  {
    fwrite(header, 1, 54, out);
    steg(in, out, header);
  }
  fclose(out);
  fclose(in);

  return 0;


}
