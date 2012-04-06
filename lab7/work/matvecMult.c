/*************************************************************************************************\
  matvecMult.c

  Author:   Tim Dunn 
  Date:     Apr 4, 2012
  Revision: 1.0
  For the Spring 2012 CSCI 4576/5576 HPSC class

  Serial base code for a very simple matrix-vector multiply intended for conversion to an
  OpenMP application. Code in almost as simplest c as possible to meet the coding level for
  any student in the class.

  Purpose: Preforms the actual matrix-vector multiply (An = Bmxn * cn) as well as matric and 
           vector dump fxns.
           
\*************************************************************************************************/
#include "proto.h"

/*************************************************************************************************\
* Function: matvectMultiply()
* Purpose: Performs the matrixs vector multiple (An = Bmxn * Cn)
* Args:
*       double* vecA = pointer to result vector A
*       double* matB = pointer to matrix B
*       double* vecC = pointer to vector C
*       
* Return: None
*
* Notes: None
\*************************************************************************************************/
void matvectMultiply(double* vecA, double* matB, double* vecC)
{
  int i,j;

	#pragma omp parallel for private(i,j)
   for(i=0; i < Args.m; i++)
  {
    // pre-init vecA
    vecA[i] = 0.0;  

    for(j=0; j < Args.n; j++)
    {
      vecA[i] += matB[i * Args.n + j] * vecC[j];
    }
  }
  
} 

/*************************************************************************************************\
* Function: outputVec() 
* Purpose: Prints the requested vector if verbose is enabled
* Args:
*       const char* szVec = string name of the vector to dump
*       double* vec = pointer to the vector to dump
*       
* Return: None
*
* Notes: None
\*************************************************************************************************/
void outputVec(const char* szVec, double* vec)
{
  int i;
  
  printf("%s\n",szVec);

  for(i=0; i < Args.n; i++)
    printf("%3.0f ", vec[i]);

  printf("\n\n");
}


/*************************************************************************************************\
* Function: outputMat() 
* Purpose: Prints the requested matrix if verbose is enabled
* Args:
*       const char* szVec = string name of the matrix to dump
*       double* mat = pointer to the matrix to dump
*       
* Return: None
*
* Notes: None
\*************************************************************************************************/
void outputMat(const char* szMat, double* mat)
{
  int i, j;
  
  printf("%s\n\n",szMat);

  for(i=0; i < Args.m; i++)
  {
    for(j=0; j < Args.n; j++)
    {
      printf("%3.0f ", mat[i * Args.n + j]);
    }
    printf("\n");
  }

  printf("\n");
}
