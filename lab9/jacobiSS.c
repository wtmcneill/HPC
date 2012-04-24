/*************************************************************************************************\
*  jacobiSS.c
*
*  APP:      jacobiSS
*  AUTHOR:   Tim Dunn 
*  EMail:    tdunn@colorado.edu
*  DATE:     Apr 19, 2012
*  REVISION: 1.0
*
*  PURPOSE: Simple Jacobi iteration method for solving a 2D steady state heat distribition PDE
*           Designed to experiment with PGI gpu directives for GPU accleration
*
*  NOTES: This app was written for the Spring 2012 CSCI 4576/5576 HPSC class.
*         This is a VERY niave solution to the 2D heat distribution PDE using Jacobi iteration
*         methods. Usually you let the jacobi run till convergence is met by reaching some
*         pre-defined EPSILON value. In this app it will always run maxIter times. This is
*         done to simplify understanding how gpu acceleration via PGI directives operates.
*
*   TO BUILD: First log onto the crc-gpu que on janus.
* qsub -I -q crc-gpu -l nodes=1 -l naccesspolicy=singletask -l walltime=30:00
*
*   Next get the proper dotkit for the PGI compiler
* use .pgi-12.3-testing          
*
*   Now compile it
* pgcc -ta=nvidia -Minfo -o jacobiSS jacobiSS.c
*
* 
*  TO RUN:  
*  ./jacobiSS -<GRID SIZE>
*    Where GRID_SIZE is the size of the grid (nxn) the app defaults to 1000.
*
\*************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef double *restrict *restrict GRID;

int main(int argc, char * argv[])
{
  int N;                        //GRID size (nxn)
  int i, j, t;
  int iter, maxIters;           //Convergence iterations
	double start, end, elapsed;   // for benchmarking
  double mean;                  //Avg BC value
  GRID u;                       //Old cell values
  GRID w;                       //New cell values
  
  	
  //Get the size of the GRID(nxn)
  if(argc > 1)
    N = atoi(argv[1]);
  else
    N = 1000;
  if(N <= 0) 
    N = 1000;
  
  //Allocate memory  
  u = (double**)malloc(sizeof(double*) * N);  
  w = (double**)malloc(sizeof(double*) * N);   
  for(i = 0; i < N; ++i )
  {
	  u[i] = (double*)malloc(sizeof(double) * N);
	  w[i] = (double*)malloc(sizeof(double) * N);
  }
    
  mean = 0.0;         //Init avg boundary value
  maxIters = 5000;    //Set the total iterations for convergence regardless of when its reached

  //Init boundary values
  for(i=0; i<N; i++)
  {
    u[i][0] = u[i][N-1] = u[0][i] = 100.0;
    u[N-1][i] = 0.0;
    mean += u[i][0] + u[i][N-1] + u[0][i] + u[N-1][i];
  }

  mean /= (4.0 * N);

  //Start Timer
  start = clock();
  
  //Init inner cell values
  for(i=1; i<N-1; i++)
    for(j=1; j<N-1; j++)
      u[i][j] = mean;
  
  //Calc the 2D heat steady state distribution via jacobi iteration
   for(iter=1; iter<maxIters; iter++)
  {

    for(i=1; i<N-1; i++)
      for(j=1; j<N-1; j++)
        //Calc cell i,j based on stencil neighbors 
        w[i][j]=(u[i-1][j] + u[i+1][j] + u[i][j-1] + u[i][j+1]) / 4.0;

    for(i=1; i<N-1; i++)
      for(j=1; j<N-1; j++)
		  u[i][j]=w[i][j];

  } // end for iter:maxIters
  
  end = clock();
  elapsed = (end-start)/1000000.0;
  printf("Run Time: %5.3f\n\n", elapsed);
  
	//Free memory
  if(u) free(*u);
  if(w) free(*w);
}
