/* mcparab.c - Monte Carlo Integral of x^2  Version 1.0.0            */
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

/***********************************************************/
/* mcparab estimates the definite integral of x^2          */
/* through a Monte Carlo simulation.                       */
/* Two uniform random numbers are generated 0-1            */
/* 1 million times                                         */
/* If y < x^2 then tally a counter                         */
/* The ratio of (total_counter / 1 million)                */
/* is approximately 1/3 to 3 decimal places                */
/* or approximately 0.333...                               */
/* with an expected error of 1/1000                        */
/***********************************************************/

#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include "elcg.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

int main(void)
   {
   int i;                     /* loop counter                 */
   int elcgelap;              /* elapsed time for elcg        */
   int taus2elap;             /* elapsed time for taus2       */
   int mtelap;                /* elapsed time for mt19937     */
   int ranlxelap;             /* elapsed time for ranlxd2     */
   unsigned int dttk;         /* combined date, time #ticks   */
   double tot;                /* total points                 */
   double bot;                /* 1 million points             */
   double ratio;              /* estimated 1/3                */
   time_t now;                /* current date and time        */
   clock_t clk;               /* current number of ticks      */
   clock_t elcgstart;         /* start time for elcg          */
   clock_t elcgfin;           /* end   time for elcg          */
   clock_t taus2start;        /* start time for taus2         */
   clock_t taus2fin;          /* end   time for taus2         */
   clock_t mtstart;           /* start time for mt19937       */
   clock_t mtfin;             /* end   time for mt19937       */
   clock_t ranlxstart;        /* start time for ranlxd2       */
   clock_t ranlxfin;          /* end   time for ranlxd2       */
   struct tms t;              /* structure used by times()    */
   gsl_rng *r;                /* GSL RNG structure            */
   llfmt *ll;                 /* elcg structure               */
   ll = (llfmt *) elcginit();     /* initialize the elcg structure */
   bot = 1000000.0;           /* set to 1 million             */
   /************************************************************/
   tot = 0.0;                 /* initialize total points   */
   i = (int) bot;             /* set loop counter          */
   /* get clock ticks since boot                           */
   elcgstart = times(&t);     /* start time for elcg       */
   while (i--)                /* loop 1 million times      */
      {
      double x;               /* horizontal coordinate        */
      double y;               /* vertical   coordinate        */
      double wyy;             /* y = x*x                      */
      x = elcgunif(ll);       /* uniform number 0-1           */
      y = elcgunif(ll);       /* uniform number 0-1           */
      wyy = x*x;              /* the parabolic curve          */
      if (y < wyy) tot += 1.0;   /* if below parabola, tally  */
      } /* for each point above or below a parabolic curve    */
   ratio = tot / bot;         /* calculate est. 1/3           */
   elcgfin = times(&t);       /* finish time for elcg         */
   printf("Monte Carlo Definite Integral of x^2\n");
   printf("      From zero to one\n");
   printf("      n = 1 million\n");
   printf("      Expected error is 1/1000\n");
   printf("       elcg %18.15f\n", ratio);
   /************************************************************/
   /* allocate the taus2 random number generator               */
   /************************************************************/
   r = (gsl_rng *) gsl_rng_alloc(gsl_rng_taus2);
   /* get clock ticks since boot                           */
   clk = times(&t);
   /* get date & time                                      */
   time(&now);
   /* combine date, time, and ticks into a single UINT     */
   dttk = (unsigned int) (now ^ clk);
   /* initialize the GSL taus2                             */
   /* random number generator to date,time,#ticks          */
   gsl_rng_set(r, dttk);      /* initialize taus2 seed        */
   tot = 0.0;                 /* initialize total points      */
   i = (int) bot;             /* set loop counter             */
   /* get clock ticks since boot                              */
   taus2start = times(&t);    /* start time for GSL taus2     */
   while (i--)                /* loop 1 million times         */
      {
      double x;               /* horizontal coordinate        */
      double y;               /* vertical   coordinate        */
      double wyy;             /* y = x*x                      */
      /* use the taus2 random number generator this time      */
      x = gsl_rng_uniform(r);      /* uniform number 0-1      */
      y = gsl_rng_uniform(r);      /* uniform number 0-1      */
      wyy = x*x;              /* the parabolic curve          */
      if (y < wyy) tot += 1.0;   /* if below parabola, tally  */
      } /* for each point above or below a parabolic curve    */
   ratio = tot / bot;         /* calculate est. 1/3           */
   taus2fin = times(&t);      /* finish time for GSL taus2    */
   printf("GSL   taus2 %18.15f\n", ratio);
   gsl_rng_free(r);
   /************************************************************/
   /* allocate the mt19937 random number generator             */
   /************************************************************/
   r = (gsl_rng *) gsl_rng_alloc(gsl_rng_mt19937);
   /* get clock ticks since boot                           */
   clk = times(&t);
   /* get date & time                                      */
   time(&now);
   /* combine date, time, and ticks into a single UINT     */
   dttk = (unsigned int) (now ^ clk);
   /* initialize the GSL Mersenne Twister                  */
   /* random number generator to date,time,#ticks          */
   gsl_rng_set(r, dttk);      /* initialize mt19937 seed      */
   tot = 0.0;                 /* initialize total points      */
   i = (int) bot;             /* set loop counter             */
   /* get clock ticks since boot                              */
   mtstart = times(&t);       /* start time for GSL mt19937   */
   while (i--)                /* loop 1 million times         */
      {
      double x;               /* horizontal coordinate        */
      double y;               /* vertical   coordinate        */
      double wyy;             /* y = x*x                      */
      /* use the mt19937 random number generator this time    */
      x = gsl_rng_uniform(r);      /* uniform number 0-1      */
      y = gsl_rng_uniform(r);      /* uniform number 0-1      */
      wyy = x*x;              /* the parabolic curve          */
      if (y < wyy) tot += 1.0;   /* if below parabola, tally  */
      } /* for each point above or below a parabolic curve    */
   ratio = tot / bot;         /* calculate est. 1/3           */
   mtfin = times(&t);         /* finish time for GSL mt19937   */
   printf("GSL mt19937 %18.15f\n", ratio);
   gsl_rng_free(r);
   /************************************************************/
   /* allocate the ranlxd2 random number generator         */
   r = (gsl_rng *) gsl_rng_alloc(gsl_rng_ranlxd2);
   /* get clock ticks since boot                           */
   clk = times(&t);
   /* get date & time                                      */
   time(&now);
   /* combine date, time, and ticks into a single UINT     */
   dttk = (unsigned int) (now ^ clk);
   /* initialize the GSL ranlxd2 random number generator      */
   /* to date,time,#ticks                                     */
   gsl_rng_set(r, dttk);      /* initialize ranlxd2 seed      */
   tot = 0.0;                 /* initialize total points      */
   i = (int) bot;             /* set loop counter             */
   /* get clock ticks since boot                              */
   ranlxstart = times(&t);    /* start time for GSL ranlxd2   */
   while (i--)                /* loop 1 million times         */
      {
      double x;               /* horizontal coordinate        */
      double y;               /* vertical   coordinate        */
      double wyy;             /* y = x*x                      */
      /* use the ranlxd2 random number generator this time    */
      x = gsl_rng_uniform(r);      /* uniform number 0-1      */
      y = gsl_rng_uniform(r);      /* uniform number 0-1      */
      wyy = x*x;              /* the parabolic curve          */
      if (y < wyy) tot += 1.0;   /* if below parabola, tally  */
      } /* for each point above or below a parabolic curve    */
   ratio = tot / bot;         /* calculate est. 1/3           */
   ranlxfin = times(&t);      /* finish time for GSL ranlxd2   */
   printf("GSL ranlxd2 %18.15f\n", ratio);
   printf("     Actual %18.15f\n", 1.0 / 3.0);
   elcgelap  = elcgfin  - elcgstart;
   taus2elap = taus2fin - taus2start;
   mtelap    = mtfin    - mtstart;
   ranlxelap = ranlxfin - ranlxstart;
   printf("       elcg ticks %6d\n", elcgelap);
   printf("GSL   taus2 ticks %6d\n", taus2elap);
   printf("GSL mt19937 ticks %6d\n", mtelap);
   printf("GSL ranlxd2 ticks %6d\n", ranlxelap);
   gsl_rng_free(r);
   free(ll->state);
   free(ll);
   return(0);
   } /* main */
