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

cannon_multiply(int n, double *a, double *b, double *c,MPI_Comm comm){
	int i; 
	int nlocal; 
	int nprocs, dims[2], periods[2]; 
	int myrank, my2drank, mycoords[2]; 
	int uprank, downrank, leftrank, rightrank, coords[2]; 
	int shiftsource, shiftdest;
	MPI_Status status; 
	MPI_Comm comm_2d; 
 
	MPI_Comm_size(comm, &nprocs); 
	MPI_Comm_rank(comm, &myrank); 
 
	/* Set up the Cartesian topology */ 
	dims[0] = dims[1] = sqrt(nprocs); 
 
	periods[0] = periods[1] = 1; 
 
	MPI_Cart_create(comm, 2, dims, periods, 1, &comm_2d); //make topology
 
	MPI_Comm_rank(comm_2d, &my2drank); //get rank & coords for topology
	MPI_Cart_coords(comm_2d, my2drank, 2, mycoords); 
 
	MPI_Cart_shift(comm_2d, 0, -1, &rightrank, &leftrank); //get ranks of desired shifts
	MPI_Cart_shift(comm_2d, 1, -1, &downrank, &uprank); 
 
	nlocal = n/dims[0]; //dimensions of local block
 
	//initial matrix alignment for A then B
	MPI_Cart_shift(comm_2d, 0, -mycoords[0], &shiftsource, &shiftdest); 
	MPI_Sendrecv_replace(a, nlocal*nlocal, MPI_DOUBLE, shiftdest, 1, shiftsource, 1, comm_2d, &status); 
 
	MPI_Cart_shift(comm_2d, 1, -mycoords[1], &shiftsource, &shiftdest); 
	MPI_Sendrecv_replace(b, nlocal*nlocal, MPI_DOUBLE, shiftdest, 1, shiftsource, 1, comm_2d, &status); 
 
	for (i=0; i<dims[0]; i++) { //main computing loop
		serial_multiply(nlocal, a, b, c); //c=c+a*b
		MPI_Sendrecv_replace(a, nlocal*nlocal, MPI_DOUBLE,leftrank, 1, rightrank, 1, comm_2d, &status);//shift left
		MPI_Sendrecv_replace(b, nlocal*nlocal, MPI_DOUBLE,uprank, 1, downrank, 1, comm_2d, &status);//shift right
	} 
 
	//restore distribution of a and b 
	MPI_Cart_shift(comm_2d, 0, +mycoords[0], &shiftsource, &shiftdest); 
	MPI_Sendrecv_replace(a, nlocal*nlocal, MPI_DOUBLE,shiftdest, 1, shiftsource, 1, comm_2d, &status); 
 
	MPI_Cart_shift(comm_2d, 1, +mycoords[1], &shiftsource, &shiftdest); 
	MPI_Sendrecv_replace(b, nlocal*nlocal, MPI_DOUBLE,shiftdest, 1, shiftsource, 1, comm_2d, &status); 
 
	MPI_Comm_free(&comm_2d);
}

void fill(double* mat,int n,double val){
	int i;
	for(i=0;i<n*n;i++){mat[i]=val;}
}

void fill_random(double* mat,int n){
	int i;
	for(i=0;i<n*n;i++){mat[i]=RandVal;}
}

int main(int argc, char* argv[]) {
	int nprocs,myrank;
	int verbose=0,print=0;
	int i;
	int n=0;//problem size
	double begin_time_p, begin_time_p_warmup, begin_time_t, end_time, end_time_warmup, serial_end, interval_p, interval_t, interval_s;
		
	MPI_Init(&argc, &argv);
	begin_time_t = MPI_Wtime();//start total time timer
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs); 
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
	//parse command line arguments
	if(myrank == 0){
		for (i = 1; i < argc; i++){ //Skip argv[0] (program name)
			if (strcmp(argv[i], "-verbose") == 0){
				verbose = 1;
			}else if(strcmp(argv[i], "-print") == 0){
				print=1;
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
	
	if(myrank == 0){
		fill_random(a,n);//fill in starting values
		fill_random(b,n);
	}fill(c,n,0);
	
	MPI_Bcast(&a, n*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);//send matricies to all processes
	MPI_Bcast(&b, n*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	//MPI_Bcast(&c, n*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);//dont send c because all procs can fill with zeroes
	
	if(print == 1){
		printf("A:\n");
		print_matrix(a,n,n);
		printf("B:\n");
		print_matrix(b,n,n);
	}
	
	interval_s=MPI_Wtime()-begin_time_t;//serial portion
	
	begin_time_p_warmup = MPI_Wtime();//run once to "warm up" the timer
	//dont run it here, bc any MPI calls after running it hang (why?)
	end_time_warmup = MPI_Wtime();
	
	begin_time_p = MPI_Wtime();
	cannon_multiply(n,&a,&b,&c,MPI_COMM_WORLD);
	end_time = MPI_Wtime();
	
	if(print == 1){
		printf("C:\n");
		print_matrix(c,n,n);
	}
	
	interval_p = end_time - begin_time_p;
	interval_t = end_time - begin_time_t - (end_time_warmup-begin_time_p_warmup);//subtract the time taken for warmup
	if(verbose == 1){
		printf("%i %i %f %f %f\n",nprocs,n,interval_t,interval_p,interval_s);
	}
	
	MPI_Abort(MPI_COMM_WORLD,0);//there might be an unfinished transfer somewhere, so barrier & finalize hang
	//MPI_Barrier(MPI_COMM_WORLD);
	//MPI_Finalize();//hangs for some reason!
	//if(myrank == 0){printf("finalized\n");}
}
