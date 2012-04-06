/*************************************************************************************************\
  getArgs.c

  Author:   Tim Dunn 
  Date:     Apr 4, 2012
  Revision: 1.0
  For the Spring 2012 CSCI 4576/5576 HPSC class

  Serial base code for a very simple matrix-vector multiply intended for conversion to an
  OpenMP application. Code in almost as simplest c as possible to meet the coding level for
  any student in the class.

  Purpose: Parses the commandline args into an Args struct. Also does the -h usage dump.
  
\*************************************************************************************************/
#include "proto.h"


/*************************************************************************************************\
* Function: get_Args()
* Purpose:  Read and init cmndln args into Args struct 
* Args:
*       int argc = arg count
*       char** argv = ag list
*
* Return:
*       int = error return code (0 = no err, 1 = error)
*
* Notes:  None
\*************************************************************************************************/
int getArgs(int argc, char** argv)
{
  int err = 0;
  int i;

  //Pre-init Args
  Args.m = 0;
  Args.n = 0;
  Args.verbose = 0;

  //Parse the cmndln args into the Args struct
  for(i = 1; i < argc; i++) 
  {
    if(argv[i][0] == '-') 
    {
      switch (argv[i][1]) 
      {
        case 'm':   // # rows
          Args.m = atoi(argv[++i]);
          break;
        case 'n':   // # columns
          Args.n = atoi(argv[++i]);
          break;
        case 'v':   // Verbose 
          Args.verbose = 1;
          break;
        case 'h':
        default: // Help (Usage)
          usage();
          err = 1;
          break;
      }
    }
  }

  //Check that there are args for m and n!
  if(!Args.m && !err)
  {
    perror("Error: Need an int value for m");
    err = 1;
  }

  if(!Args.n && !err)
  {
    perror("Error: Need an int value for n");
    err = 1;
  }

  return err;
}

/*************************************************************************************************\
* Function: usage()
* Purpose: Displays cmndln usage for running the app 
* Args: None
*       
* Return: None
*
* Notes: None
\*************************************************************************************************/
void usage()
{
  printf("\n");
  printf("%s [-m] [-n] -s -v -h\n", (APP_NAME));
  printf("     -m #: Number of rows (Required!)\n");
  printf("     -n #: Number of columns (Required!)\n");
  printf("     -v #: Enable verbose\n");
  printf("     -h  : Display this.\n");
  printf("\n");
  printf(" demo:  ./%s -m 10 -n 10 -v\n\n", (APP_NAME));
}
