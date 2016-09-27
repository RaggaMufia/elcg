/* elcgbit.c - elcg random bit generator Version 1.0.0               */
/* Copyright (C) 2016 aquila62 at github.com                         */

/* This program is free software; you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License as    */
/* published by the Free Software Foundation; either version 2 of    */
/* the License, or (at your option) any later version.               */

/* This program is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of    */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the      */
/* GNU General Public License for more details.                      */

/* You should have received a copy of the GNU General Public License */
/* along with this program; if not, write to:                        */

   /* Free Software Foundation, Inc.                                 */
   /* 59 Temple Place - Suite 330                                    */
   /* Boston, MA 02111-1307, USA.                                    */

#include "elcg.h"

int elcgbit(llfmt *ll)
   {
   int i;
   int bit;
   int tmpout;
   unsigned int *p;           /* state pointer */
   unsigned int tmp;          /* used for bays-durham shuffle */
   /***********************************************************/
   /* back up the previous two states                         */
   /***********************************************************/
   ll->ofst  = (ll->pprev >> 22) & 0x3ff;   /* offset into state array  */
   ll->pprev = ll->prev;   /* prev ==> prev prev              */
   ll->prev  = ll->out;    /* current ==> prev                */
   /***********************************************************/
   /* Calculate the LCG algorithm inline to save a call       */
   /* to elcg()                                               */
   /***********************************************************/
   /* The LCG macro is in elcg.h                              */
   /* XOR the previous two results with the current output    */
   LCG;
   ll->out = ((ll->x >> 16) ^ ll->pprev ^ ll->prev);
   /***********************************************************/
   /* Bays-Durham shuffle of state array                      */
   /* 1024! = 5.41e+2639 base 10 rounded down                 */
   /* The period length of the state array is very long       */
   /***********************************************************/
   /* point into the state array */
   p       = (unsigned int *) ll->state + ll->ofst;
   /* swap the current output with a member in the state array */
   tmp     = *p;
   *p      = ll->out;
   ll->out = tmp;
   /********************************************************/
   /* xor all 32 bits in ll->out to get one output bit     */
   /* the output from elcgbit() performs very well in      */
   /* dieharder -a and TestU01 Crush tests                 */
   /********************************************************/
   bit = 0;           /* initialize the output bit */
   tmpout = ll->out;  /* temporary copy of output */
   i = 32;            /* loop counter for all 32 bits */
   while (i--)        /* loop 32 times */
      {
      bit ^= tmpout;  /* xor the low order bit to the output bit */
      tmpout >>= 1;         /* shift to the next lower order bit */
      } /* for each bit in ll->out */
   /* return output bit */
   return(bit & 1);
   } /* elcgbit subroutine */
