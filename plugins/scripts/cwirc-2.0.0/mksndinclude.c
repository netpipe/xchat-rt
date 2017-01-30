/* This program takes a mono / 16 bit WAV file on its standard input and
   generate a C include file on its standard output containing all the samples.
   It does not do any error checking : if the format isn't exactly the one
   above, the resulting samples in the include file will be wrong. The name of
   the resulting resulting variable defined in the include file is passed as
   first argument.

   (c) Pierre-Philippe Coupard - 15/08/2003

   This program is distributed under the terms of the GNU General Public License
   See the COPYING file for details
*/
#include <stdio.h>

#include "types.h"



/* Main program */
int main(int argc,char *argv[])
{
  T_BOOL little_endian=0;
  T_U8 c1,c2;
  T_S16 sample;
  T_U8 *sptr;
  long sampleno=0;
  int i;

  sptr=(T_U8 *)&sample;

  /* Do the endianness test */
  i=1;
  if(((char *)&i)[0])
    little_endian=1;

  /* Read and ignore 44 bytes (the header) */
  for(i=0;i<44;i++)
    getc(stdin);

  printf("static const T_S16 %s[]={\n",argv[1]);

  /* Read and convert the samples */
  while(!feof(stdin))
  {
    c1=getc(stdin);
    c2=getc(stdin);

    if(little_endian)
    {
      sptr[0]=c1;
      sptr[1]=c2;
    }
    else
    {
      sptr[0]=c2;
      sptr[1]=c1;
    }

    if(sampleno>0)
    {
      printf(",");
      if(sampleno%8==0)
        printf("\n");
    }

    printf("%d",sample);
    sampleno++;
  }
  printf("};\n\n");

  printf("static const long %s_nbsamples=%ld;\n",argv[1],sampleno);

  return(0);
}
