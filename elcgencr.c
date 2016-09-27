/************************************************************************/
/* elcgencr.c  Version 1.0.0                                            */
/* Encrypt an input stream with the elcg algorithm                      */
/* Copyright (C) 2016  aquila62 at github.com                           */

/* This program is free software; you can redistribute it and/or        */
/* modify it under the terms of the GNU General Public License as       */
/* published by the Free Software Foundation; either version 2 of       */
/* the License, or (at your option) any later version.                  */

/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */

/* You should have received a copy of the GNU General Public License    */
/* along with this program; if not, write to:                           */

	/* Free Software Foundation, Inc.         */
	/* 59 Temple Place - Suite 330            */
	/* Boston, MA  02111-1307, USA.           */

/**********************************************************/
/* This program initializes the elcg random number        */
/* generator state by overriding its state with RANDU.    */
/* RANDU is initialized by the CRC of the input password  */
/* parameter.                                             */
/* RANDU is a weak random number generator, but it is     */
/* satisfactory enough for the encryption program to      */
/* pass the dieharder random number test suite.           */
/* Once initialized with RANDU, elcg is warmed up         */
/* to minimize the weakness behind its initialization     */
/* routine.                                               */
/**********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "elcg.h"

/* length of input/output buffer */
#define BUFSZ (1024*1024)

/* RANDU algorithm */
#define RANDU (seed = seed * 65539)

/* print the command syntax */
void putstx(char *pgm)
   {
   fprintf(stderr,"Usage: %s password\n", pgm);
   fprintf(stderr,"Where password is an ASCII string\n");
   fprintf(stderr,"Example: %s abcd\n", pgm);
   fprintf(stderr,"Example: %s \"abcd efgh\"\n", pgm);
   fprintf(stderr,"Example: %s 'abcd efgh'\n", pgm);
   exit(1);
   } /* putstx */

/**************************************/
/* read one block of input from stdin */
/* return the length of the block     */
/* a partial block may be returned    */
/* maximum blocksize is BUFSZ         */
/* input may be binary data           */
/**************************************/
int getblk(unsigned char *blk)
   {
   int len;
   len = read(0,blk,BUFSZ);
   if (!len) return(EOF);
   if (len < 0)
      {
      fprintf(stderr,"getblk: read error\n");
      exit(1);
      } /* read error */
   return(len);
   } /* getblk */

/***************************************/
/* write one block of output to stdout */
/* maximum blocksize is determined by  */
/* the calling routine.                */
/* output is binary data               */
/***************************************/
void putblk(unsigned char *blk, int len)
   {
   int rslt;
   rslt = write(1,blk,len);
   if (rslt != len)
      {
      fprintf(stderr,"putblk: write error\n");
      exit(1);
      } /* write error */
   } /* putblk */

/* Generate the CRC32C table */
void bldtbl(unsigned int *table)
   {
   int j;
   unsigned int byte;
   unsigned int crc;
   unsigned int mask;
   for (byte = 0; byte <= 255; byte++)
      {
      crc = byte;
      for (j = 7; j >= 0; j--)
         {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
         } /* for each bit in byte */
      table[byte] = crc;
      } /* for each of 256 bytes in table */
   } /* bldtbl */

/* Calculate the 32-bit CRC of a message */
unsigned int crc32c(unsigned char *message,
   int len, unsigned int *table)
   {
   unsigned char *p,*q;
   unsigned int byte, crc;
   p = (unsigned char *) message;
   q = (unsigned char *) message + len;
   crc = 0xFFFFFFFF;
   while (p < q)
      {
      byte = *p++;
      crc = (crc >> 8) ^ table[(crc ^ byte) & 0xFF];
      } /* for each byte in message */
   return ~crc;
   } /* crc32c */

int main(int argc, char **argv)
   {
   int i;                      /* loop counter */
   int pswlen;                 /* length of password parameter */
   unsigned int crc;           /* crc output of crc32c */ 
   unsigned int seed;          /* RANDU 32-bit seed */
   unsigned int tbl[256];      /* CRC table */
   unsigned long long *statep;       /* pointer into ll->state */
   unsigned long long *stateq;       /* pointer for end of ll->state */
   unsigned char psw[128];     /* password text */
   unsigned char *buf;         /* input/output buffer */
   llfmt *ll;                  /* elcg RNG structure */
   if (argc != 2) putstx(*argv);    /* must have password parameter */
   /*******************************/
   /* validate length of password */
   /*******************************/
   pswlen = strlen(*(argv+1));    /* validate length of password */
   if (pswlen > 64)
      {
      fprintf(stderr,"main: password overflow\n");
      fprintf(stderr,"password is 1-64 bytes long\n");
      putstx(*argv);
      } /* password overflow */
   strcpy((char *) psw,*(argv+1));     /* save password */
   /*****************************************************/
   /* Alloocate memory for the input/output buffer      */
   /*****************************************************/
   buf = (unsigned char *) malloc(BUFSZ);
   if (buf == NULL)
      {
      fprintf(stderr,"main: out of memory "
         "allocating buf\n");
      exit(1);
      } /* out of memory */
   /*****************************************************/
   /* initialize random number generator elcg           */
   /* initialize the CRC table                          */
   /* calculate the CRC of the password parameter       */
   /* initialize the RANDU seed to this CRC             */
   /*****************************************************/
   ll = (llfmt *) elcginit();    /* initialize elcg RNG */
   bldtbl(tbl);      /* initialize CRC table */
   seed = crc32c(psw,strlen((char *) psw),tbl);
   /*****************************************************/
   /* warm up the RANDU random number generator         */
   /*****************************************************/
   i = 256;
   while (i--) RANDU;
   /*****************************************************/
   /* override the state of the random number generator */
   /* with the password parameter converted into        */
   /* 32-bit unsigned integers.                         */
   /*****************************************************/
   RANDU;
   crc = crc32c((unsigned char *) &seed,4,tbl);
   ll->x = (unsigned long long) crc;
   ll->x     = (ll->x << 16) | 0x330e;
   RANDU;
   crc = crc32c((unsigned char *) &seed,4,tbl);
   ll->prev = (unsigned long long) crc;
   ll->prev = (ll->prev << 16) | 0x330e;
   RANDU;
   crc = crc32c((unsigned char *) &seed,4,tbl);
   ll->pprev = (unsigned long long) crc;
   ll->pprev = (ll->pprev << 16) | 0x330e;
   statep = (unsigned long long *) ll->state;
   stateq = (unsigned long long *) ll->state + 1024;
   while (statep < stateq)
      {
      RANDU;
      crc = crc32c((unsigned char *) &seed,4,tbl);
      *statep = (unsigned long long) crc;
      *statep = (*statep << 16) | 0x330e;
      statep++;
      } /* for each state in ll->state array */
   /*****************************************************/
   /* warm up the elcg random number generator          */
   /*****************************************************/
   i = 2048;
   while (i--) elcg(ll);
   /*****************************************************/
   /* encrypt the stdin data stream                     */
   /* write encrypted data to stdout                    */
   /* loop terminated by end of input stream            */
   /*****************************************************/
   while (1)
      {
      int blklen;
      unsigned char *p,*q;
      blklen = getblk(buf);
      if (blklen == EOF) break;
      p = (unsigned char *) buf;
      q = (unsigned char *) buf + blklen;
      while (p < q)
         {
         *p = *p ^ elcgpwr(ll,8);
	 p++;
	 } /* for each byte in the buffer */
      putblk(buf,blklen);
      } /* for each block encrypted */
   free(buf);
   free(ll->state);
   free(ll);
   return(0);
   } /* main */
