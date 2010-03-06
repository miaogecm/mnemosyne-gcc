///-*-C++-*-//////////////////////////////////////////////////////////////////
//
// Hoard: A Fast, Scalable, and Memory-Efficient Allocator
//        for Shared-Memory Multiprocessors
// Contact author: Emery Berger, http://www.cs.utexas.edu/users/emery
//
// Copyright (c) 1998-2000, The University of Texas at Austin.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as
// published by the Free Software Foundation, http://www.fsf.org.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
//////////////////////////////////////////////////////////////////////////////


/*
  threadtest

  This program does nothing but generate a number of kernel threads
  that allocate and free memory, with a variable
  amount of "work" (i.e. cycle wasting) in between.
*/

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <iostream.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "arch-specific.h"
#include "timer.h"

int niterations = 50;	// Default number of iterations.
int nobjects = 300000;  // Default number of objects.
int nthreads = 1;	// Default number of threads.
int work = 0;		// Default number of loop iterations.
int size = 1;


class Foo {
public:
  Foo (void)
    : x (14),
      y (29)
    {}

  int x;
  int y;
};


extern "C" void * worker (void *)
{
  int i, j;
  Foo ** a;
  a = new Foo * [nobjects / nthreads];

  for (j = 0; j < niterations; j++) {

    // printf ("%d\n", j);
    for (i = 0; i < nobjects / nthreads; i++) {
      a[i] = new Foo[size];
      for (volatile int d = 0; d < work; d++) {
	volatile int f = 1;
	f = f + f;
	f = f * f;
	f = f + f;
	f = f * f;
      }
      assert (a[i]);
    }
    
    for (i = 0; i < nobjects / nthreads; i++) {
      delete a[i];
      for (volatile int d = 0; d < work; d++) {
	volatile int f = 1;
	f = f + f;
	f = f * f;
	f = f + f;
	f = f * f;
      }
    }
  }

  delete [] a;

  return NULL;
}

#if defined(__sgi)
#include <ulocks.h>
#endif

int main (int argc, char * argv[])
{
  hoardThreadType * threads;
  
  if (argc >= 2) {
    nthreads = atoi(argv[1]);
  }

  if (argc >= 3) {
    niterations = atoi(argv[2]);
  }

  if (argc >= 4) {
    nobjects = atoi(argv[3]);
  }

  if (argc >= 5) {
    work = atoi(argv[4]);
  }

  if (argc >= 6) {
    size = atoi(argv[5]);
  }

  printf ("Running threadtest for %d threads, %d iterations, %d objects, %d work and %d size...\n", nthreads, niterations, nobjects, work, size);

  threads = new hoardThreadType[nthreads];
  hoardSetConcurrency (nthreads);

  Timer t;

  t.start ();

  int i;
  for (i = 0; i < nthreads; i++) {
    hoardCreateThread (threads[i], worker, NULL);
  }

  for (i = 0; i < nthreads; i++) {
    hoardJoinThread (threads[i]);
  }
  t.stop ();

  printf( "Time elapsed = %f\n", (double) t);

  delete [] threads;

  return 0;
}