#include<mpi.h>

MatrixMatrixMultiply(int n, double *a, double *b, double *c, 
                      MPI_Comm comm) 
 { 
   int i; 
   int nlocal; 
   int npes, dims[2], periods[2]; 
   int myrank, my2drank, mycoords[2]; 
   int uprank, downrank, leftrank, rightrank, coords[2]; 
   int shiftsource, shiftdest; 
   MPI_Status status; 
   MPI_Comm comm_2d; 

   /* Get the communicator related information */ 
   MPI_Comm_size(comm, &npes); 
   MPI_Comm_rank(comm, &myrank); 

   /* Set up the Cartesian topology */ 
   dims[0] = dims[1] = sqrt(npes); 

   /* Set the periods for wraparound connections */ 
   periods[0] = periods[1] = 1; 

   /* Create the Cartesian topology, with rank reordering */ 
   MPI_Cart_create(comm, 2, dims, periods, 1, &comm_2d); 

   /* Get the rank and coordinates with respect to the new topology */ 
   MPI_Comm_rank(comm_2d, &my2drank); 
   MPI_Cart_coords(comm_2d, my2drank, 2, mycoords); 

   /* Compute ranks of the up and left shifts */ 
   MPI_Cart_shift(comm_2d, 0, -1, &rightrank, &leftrank); 
   MPI_Cart_shift(comm_2d, 1, -1, &downrank, &uprank); 

   /* Determine the dimension of the local matrix block */ 
   nlocal = n/dims[0]; 

   /* Perform the initial matrix alignment. First for A and then for B */ 
   MPI_Cart_shift(comm_2d, 0, -mycoords[0], &shiftsource, &shiftdest); 
   MPI_Sendrecv_replace(a, nlocal*nlocal, MPI_DOUBLE, shiftdest, 
       1, shiftsource, 1, comm_2d, &status); 

   MPI_Cart_shift(comm_2d, 1, -mycoords[1], &shiftsource, &shiftdest); 
   MPI_Sendrecv_replace(b, nlocal*nlocal, MPI_DOUBLE, 
       shiftdest, 1, shiftsource, 1, comm_2d, &status); 

   /* Get into the main computation loop */ 
   for (i=0; i<dims[0]; i++) { 
     MatrixMultiply(nlocal, a, b, c); /*c=c+a*b*/ 

     /* Shift matrix a left by one */ 
     MPI_Sendrecv_replace(a, nlocal*nlocal, MPI_DOUBLE, 
         leftrank, 1, rightrank, 1, comm_2d, &status); 

     /* Shift matrix b up by one */ 
     MPI_Sendrecv_replace(b, nlocal*nlocal, MPI_DOUBLE, 
         uprank, 1, downrank, 1, comm_2d, &status); 
   } 

   /* Restore the original distribution of a and b */ 
   MPI_Cart_shift(comm_2d, 0, +mycoords[0], &shiftsource, &shiftdest); 
   MPI_Sendrecv_replace(a, nlocal*nlocal, MPI_DOUBLE, 
       shiftdest, 1, shiftsource, 1, comm_2d, &status); 

   MPI_Cart_shift(comm_2d, 1, +mycoords[1], &shiftsource, &shiftdest); 
   MPI_Sendrecv_replace(b, nlocal*nlocal, MPI_DOUBLE, 
       shiftdest, 1, shiftsource, 1, comm_2d, &status); 

   MPI_Comm_free(&comm_2d); /* Free up communicator */ 
 } 

 /* This function performs a serial matrix-matrix multiplication c = a*b */ 
 MatrixMultiply(int n, double *a, double *b, double *c) 
 { 
   int i, j, k; 

   for (i=0; i<n; i++) 
     for (j=0; j<n; j++) 
       for (k=0; k<n; k++) 
         c[i*n+j] += a[i*n+k]*b[k*n+j]; 
}
