/*************************************************************************************************\
  proto.h

  Author:   Tim Dunn 
  Date:     Apr 4, 2012
  Revision: 1.0
  For the Spring 2012 CSCI 4576/5576 HPSC class

  Serial base code for a very simple matrix-vector multiply intended for conversion to an
  OpenMP application. Code in almost as simplest c as possible to meet the coding level for
  any student in the class.

  Purpose: Declares all the variables and functions for the application.
  
\*************************************************************************************************/
#ifndef PROTO_H
#define PROTO_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define APP_NAME ("omp_MatVecMultiply")                                       //.exe name 
#define RNDRANGE(max) (double)(((double)rand() / (double)RAND_MAX) *(max));   // Ranged random num generator macro
#define ELAPSEDTIME(t1, t2) ((double)t2-(double)t1)/100000.0;                 // Calc the elapsedd time


// Global Args struct for cmndln args
struct global_argData
{
  int m;          // # rows
  int n;          // # columns
  int verbose;    // Bool to print out verbose information
}
 Args;


/////////////////////////////////////////////////
// main.c var and fxn decalrations
/////////////////////////////////////////////////
void errExit(const char* str);


/////////////////////////////////////////////////
// matvecMult.c var and fxn decalrations
/////////////////////////////////////////////////
//void matvectMultiply(double* vecA, double* matB, double* vecC);
void matvectMultiply(double* vecA, double* matB, double* vecC);
void outputVec(const char* szVec, double* vec);
void outputMat(const char* szMat, double* mat);


/////////////////////////////////////////////////
// getArgs.c var and fxn decalrations
/////////////////////////////////////////////////
int getArgs(int argc, char** argv);
void usage();


#endif // PROTO_H
