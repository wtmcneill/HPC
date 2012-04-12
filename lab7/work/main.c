/*************************************************************************************************\
  main.c

  Author:   Tim Dunn 
  Date:     Apr 4, 2012
  Revision: 1.0
  For the Spring 2012 CSCI 4576/5576 HPSC class

  Serial base code for a very simple matrix-vector multiply intended for conversion to an
  OpenMP application. Code in almost as simplest c as possible to meet the coding level for
  any student in the class.

  Purpose: Contains the main() and a remdial error msg/exit function. Note: There is a potential 
           of memory leaks if vecA, matB, and/or vecC exist. This can be emeadied easy enough 
           but is left to the user if it is desired.

\*************************************************************************************************/
#include "proto.h"


int main(int argc, char *argv[])
{
  int err = 0;
  int i, j;
  double *vecA, *matB, *vecC;
  double s_time,e_time;
    
  // Get cmndln args exit if error occurs
  if((err = getArgs(argc, argv)))
    errExit("Exiting app");

 //Args.m = 3;
 // Args.n = 3;
 // Args.verbose = 1;

  // Allocate memory for the matrix and vectors
  if(! (vecA = (double*) malloc(Args.m * sizeof(double))))
    errExit("Error: Failed to allocate memory for vecA\n");
  
  if(! (matB = (double*) malloc(Args.m * Args.n * sizeof(double))))
    errExit("Error: Failed to allocate memory for matB\n");
  
  if(! (vecC = (double*) malloc(Args.n * sizeof(double))))
    errExit("Error: Failed to allocate memory for vecC\n");
  
  // Seed the random number generator (This provides a repeatable random series)
  srand(1);

  // Init matB as an array with random values between 1 and 10
  for(i=0; i < Args.m; i++)
  {
    for(j=0; j < Args.n; j++)
       matB[i * Args.n + j] = RNDRANGE(10.0);
  }

  // Init vecC as our vector which multiples matB*2
  for(i=0; i < Args.n; i++)
    vecC[i] = RNDRANGE(10.0);

  // Perform the matrix-vector multiply
  s_time=omp_get_wtime();
  matvectMultiply(vecA, matB, vecC);
  e_time=omp_get_wtime();

  // If verbose then display the matrices
  if(Args.verbose)
  {
    //outputMat("Matrix B:", matB);
    //outputVec("Vector C:", vecC);
    //outputVec("Resultant Vector A:", vecA);
    printf("Multiply time: %f %d\n",e_time-s_time,omp_get_max_threads());
  }

  // Free memory 
  if(vecA) free(vecA);
  if(matB) free(matB);
  if(vecC) free(vecC);
  
  //system("pause");
  return err;
}


/*************************************************************************************************\
* Function: errExit 
* Purpose: Remedial error msg/exit fxn called when a critical error is incurred by the app
* Args:
*       const char* str = Error msg to display befor exit
*       
* Return: None
*
* Notes: There is a potential of memory leaks if vecA, matB, and/or vecC exist. This can be 
*        remeadied easy enough but is left to the user if it is desired.
\*************************************************************************************************/
void errExit(const char* str)
{
  printf("%s", str);
  exit(1);
}
