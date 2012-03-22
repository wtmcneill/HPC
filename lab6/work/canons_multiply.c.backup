#include<mpi.h>
#include<stdlib.h>
#include<math.h>
#include<stdio.h>

#define RandVal  rand()%10

serial_multiply(int n, double *a, double *b, double *c){//serial matrix-matrix multiplication c = a*b
	int i, j, k; 

	for (i=0; i<n; i++){
		for (j=0; j<n; j++){
			for (k=0; k<n; k++){
				c[i*n+j] += a[i*n+k]*b[k*n+j];
			}
		}
	}
}

void print_row(double* row,int size){
	int j;
	for(j=0;j<size;j++){
		printf("%f ",row[j]);
	}printf("\n");
}

void print_matrix(double* mat,int n_rows,int row_length){
	int i;
	for(i=0;i<n_rows;i++){
		printf("Row %i: ",i);
		print_row(&(mat[row_length*i]),row_length);
	}
}

cannon_multiply(int n, double *a, double *b, double *c, MPI_Comm comm){
	int i, j, nlocal; 
	double *a_buffers[2], *b_buffers[2]; 
	int nprocs, dims[2], periods[2]; 
	int myrank, my2drank, mycoords[2]; 
	int uprank, downrank, leftrank, rightrank, coords[2]; 
	int shiftsource, shiftdest; 
	MPI_Status status; 
	MPI_Comm comm_2d; 
	MPI_Request reqs[4]; 

	MPI_Comm_size(comm, &nprocs); 
	MPI_Comm_rank(comm, &myrank);

	dims[0] = dims[1] = sqrt(nprocs); //Set up the Cartesian topology

	periods[0] = periods[1] = 1; //Set the periods for wraparound connections

	MPI_Cart_create(comm, 2, dims, periods, 1, &comm_2d); //Create the Cartesian topology, with rank reordering

	MPI_Comm_rank(comm_2d, &my2drank); //Get the rank and coordinates with respect to the new topology
	MPI_Cart_coords(comm_2d, my2drank, 2, mycoords); 

	MPI_Cart_shift(comm_2d, 0, -1, &rightrank, &leftrank); //Compute ranks of the up and left shifts
	MPI_Cart_shift(comm_2d, 1, -1, &downrank, &uprank); 

	nlocal = n/dims[0]; //Determine the dimension of the local matrix block

	a_buffers[0] = a; //Setup the a_buffers and b_buffers arrays
	a_buffers[1] = (double *)malloc(nlocal*nlocal*sizeof(double)); 
	b_buffers[0] = b; 
	b_buffers[1] = (double *)malloc(nlocal*nlocal*sizeof(double)); 

	//initial matrix alignment. for A then B
	MPI_Cart_shift(comm_2d, 0, -mycoords[0], &shiftsource, &shiftdest); 
	MPI_Sendrecv_replace(a_buffers[0], nlocal*nlocal, MPI_DOUBLE, shiftdest, 1, shiftsource, 1, comm_2d, &status); 

	MPI_Cart_shift(comm_2d, 1, -mycoords[1], &shiftsource, &shiftdest); 
	MPI_Sendrecv_replace(b_buffers[0], nlocal*nlocal, MPI_DOUBLE, shiftdest, 1, shiftsource, 1, comm_2d, &status); 
	
	if(myrank == 0){printf("starting main loop, up to %i\n",dims[0]);}
	for (i=0; i<dims[0]; i++) { //main computing loop
		
		MPI_Isend(a_buffers[i%2], nlocal*nlocal, MPI_DOUBLE, leftrank, 1, comm_2d, &reqs[0]); 
		MPI_Isend(b_buffers[i%2], nlocal*nlocal, MPI_DOUBLE, uprank, 1, comm_2d, &reqs[1]); 
		MPI_Irecv(a_buffers[(i+1)%2], nlocal*nlocal, MPI_DOUBLE, rightrank, 1, comm_2d, &reqs[2]); 
		MPI_Irecv(b_buffers[(i+1)%2], nlocal*nlocal, MPI_DOUBLE, downrank, 1, comm_2d, &reqs[3]); 

		serial_multiply(nlocal, a_buffers[i%2], b_buffers[i%2], c); //c=c+a*b

		if(myrank == 0){printf("waiting for communications on iteration %i\n",i);}
		for (j=0; j<4; j++){MPI_Wait(&reqs[j], &status);}
	} 
	
	if(myrank == 0){printf("done loop, restoring distributions\n");}
	
	//restore original distribution of a & b 
	MPI_Cart_shift(comm_2d, 0, +mycoords[0], &shiftsource, &shiftdest); 
	MPI_Sendrecv_replace(a_buffers[i%2], nlocal*nlocal, MPI_DOUBLE, shiftdest, 1, shiftsource, 1, comm_2d, &status); 

	MPI_Cart_shift(comm_2d, 1, +mycoords[1], &shiftsource, &shiftdest); 
	MPI_Sendrecv_replace(b_buffers[i%2], nlocal*nlocal, MPI_DOUBLE, shiftdest, 1, shiftsource, 1, comm_2d, &status); 

	//cleanup
	MPI_Comm_free(&comm_2d);
	free(a_buffers[1]); 
	free(b_buffers[1]); 
}

void fill(double* mat,int n,double val){
	int i;
	for(i=0;i<n*n;i++){
		mat[i]=val;
	}
}

void fill_random(double* mat,int n){
	int i;
	for(i=0;i<n*n;i++){
		mat[i]=RandVal;
	}
}

int main(int argc, char* argv[]) {
	int nprocs,myrank;
	int verbose=0;
	int i;
	int n=0;//problem size
		
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs); 
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
	//parse command line arguments
	if(myrank == 0){
		for (i = 1; i < argc; i++){ //Skip argv[0] (program name)
			if (strcmp(argv[i], "-verbose") == 0){
				verbose = 1;
			}else if(strcmp(argv[i], "-size") == 0){
				if(i+1<argc){//make sure there is another argument there
					n=atoi(argv[i+1]);
				}if(n<2){
					printf("problem size too small (could be input format error). defaulting to 4");
					n=4;
				}
			}
		}
	}
	
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);//send size to all processes
	double a[n*n];//space for all the matricies
	double b[n*n];
	double c[n*n];
	
	fill_random(a,n);//fill in starting values
	fill_random(b,n);
	fill(c,n,0);
	
	if(myrank == 0){
		printf("A:\n");
		print_matrix(a,n,n);
		printf("B:\n");
		print_matrix(b,n,n);
	}
	
	if(myrank == 0){printf("starting cannons\n");}
	cannon_multiply(n,&a,&b,&c,MPI_COMM_WORLD);
	if(myrank == 0){printf("done cannons\n");}
	
	if(myrank == 0){
		printf("C:\n");
		print_matrix(c,n,n);
	}
	
	MPI_Finalize();
	if(myrank == 0){printf("finalized\n");}
}

