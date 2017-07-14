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
/*************************
 * deSteg takes a file pointer and seeks beyond the header..
 * i is used as a counter to keep track of a full char
 * mask is the mask, char & 3 to extract to lsb 
 * colorChar is the current byte, printChar is the
 * char built from the extractions, it is printed when 
 * i == 4, the actual char is full, loop breaks when*/
void deSteg(FILE *in, unsigned char *header)
{
  int i, j;
  char mask = 0;
  char colorChar = 0;
  char printChar = 0;
  int pixelWidth;
  int pixelHeight;
  int rowSize;
  int rowPadding;
  int pixelDataSize;


  pixelWidth = getIntFromArray(&header[18]);
  pixelHeight = getIntFromArray(&header[22]);
  /* compute row padding */
  rowSize = pixelWidth*3;
  rowPadding = (4 - (rowSize % 4)) % 4;
  pixelDataSize = rowSize*pixelHeight;
  rowSize += rowPadding;

  i = 0;
  fseek(in, 54, SEEK_SET);

  for(j = 0; j < pixelDataSize + rowSize; j++)
  {
    colorChar = getc(in);
    mask = colorChar & 3;
    printChar |= mask;
    i++;
    if(i >= 4)
    {
      if((printChar > 31 && printChar < 127) || printChar == 10)
      {
        putchar(printChar);
      }
      else if(printChar == 0)
      {
        break;
      }
      i = 0;
    }
    printChar <<= 2;
  }
}

/**********************************************
 * isBMP takes the pointer to header and checks if the 
 * first two chars are B M then if the header is the
 * correct size based on the data, and then if the file
 * is set for 24 bits per pixel*/
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
 * Main takes one arg. fileIn. message text printed to stdout                 *
 * in/out headers match (pass the char** around?) to maintain image size      *
 * each char of message hidden in lowest two bits of four bytes pixel data    *
 * RGB bytes and padding bytes. 
 * print each char as it is extracted (putchar?) 
 * reach null sequence EOM 0 byte... quit  */
int main(int argc, char **argv){
  char *fileIn;
  FILE *in;
  unsigned char header[54];

  if(argc != 2)
  {
    printf("wrong # args\n");
    return 1;
  }


  fileIn = argv[1];
  
  in = fopen(fileIn, "rb");/*book says b not rec by fopen p.178*/

  fread(header, 1, 54, in);/*read file header into header array */

  if(isBMP(header))
  {
    deSteg(in, header);
  }
  fclose(in);

  return 0;

}
