#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>


typedef struct GridInfo {
	int p;//Total number of processes
	MPI_Comm  comm;//Communicator for entire grid 
	MPI_Comm  row_comm;//Communicator for my row	
	MPI_Comm  col_comm;//Communicator for my col
	int q;//Order of grid
	int my_row;//My row number
	int my_col;//My column number
	int myrank;//My rank in the grid comm
} GRID_INFO_T;

#define MAX		 65536
#define RandVal  rand()%10

typedef struct LocalMatrix {
	int n_bar;
#define Order(A) ((A)->n_bar)
	float entries[MAX];
#define Entry(A,i,j) (*(((A)->entries) + ((A)->n_bar)*(i) + (j)))
} LOCAL_MATRIX_T;

/* Function Declarations */
LOCAL_MATRIX_T* Local_matrix_allocate(int n_bar);//get heap space for matrix
void Free_local_matrix(LOCAL_MATRIX_T** local_A);
void Generate_matrix(LOCAL_MATRIX_T* local_A, GRID_INFO_T* grid, int n);//fills matrix randomly
void Print_matrix(char* title, LOCAL_MATRIX_T* local_A, GRID_INFO_T* grid, int n);
void Set_to_zero(LOCAL_MATRIX_T* local_A);// fills matrix with zeroes
void Local_matrix_multiply(LOCAL_MATRIX_T* local_A, LOCAL_MATRIX_T* local_B, LOCAL_MATRIX_T* local_C);
void Build_matrix_type(LOCAL_MATRIX_T* local_A);//make new MPI type for matrix
MPI_Datatype local_matrix_mpi_t;//MPI type for matrix

LOCAL_MATRIX_T* temp_mat;

/*********************************************************/
int main(int argc, char* argv[]) {
	int nprocs; //number of processes
	int myrank; //rank of process
	GRID_INFO_T grid;  //grid info of process
	LOCAL_MATRIX_T* local_A;
	LOCAL_MATRIX_T* local_B;
	LOCAL_MATRIX_T* local_C;
	int n=-1; //order of matrix
	int n_bar; //order of submatrix
	double begin_time_p, begin_time_p_warmup, begin_time_t, end_time, end_time_warmup, serial_end, interval_p, interval_t, interval_s;
	int verbose=0,i,print=0;

	void Setup_grid(GRID_INFO_T*  grid);
	void Fox(int n, GRID_INFO_T* grid, LOCAL_MATRIX_T* local_A, LOCAL_MATRIX_T* local_B, LOCAL_MATRIX_T* local_C);

	MPI_Init(&argc, &argv);
	begin_time_t = MPI_Wtime();//start total time timer
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	
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
	
	Setup_grid(&grid);//setup the grid info of process

	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);//send size to all processes
	n_bar = n/grid.q;//size of submatrix

	local_A = Local_matrix_allocate(n_bar);//space for submatrix A in each process
	Order(local_A) = n_bar;
	//printf("GenA");
	Generate_matrix(local_A, &grid, n);
	//Print_matrix("Matrix A =", local_A, &grid, n);

	local_B = Local_matrix_allocate(n_bar);//space for submatrix B in each process
	Order(local_B) = n_bar;
	Generate_matrix(local_B, &grid, n);
	//Print_matrix("Matrix B =", local_B, &grid, n);

	Build_matrix_type(local_A);//MPI type for submatrix
	temp_mat = Local_matrix_allocate(n_bar);//space for temp matrix

	local_C = Local_matrix_allocate(n_bar);
	Order(local_C) = n_bar;
	
	interval_s=MPI_Wtime()-begin_time_t;//serial portion
	
	begin_time_p_warmup = MPI_Wtime();//run once to "warm up" the timer
	Fox(n, &grid, local_A, local_B, local_C);
	end_time_warmup = MPI_Wtime();
	
	begin_time_p = MPI_Wtime();
	Fox(n, &grid, local_A, local_B, local_C);//run foxs
	end_time = MPI_Wtime();

	if(print == 1){//only rank 0 can have print == 0
		Print_matrix("\nThe product is:", local_C, &grid, n);
	}

	interval_p = end_time - begin_time_p;
	interval_t = end_time - begin_time_t - (end_time_warmup-begin_time_p_warmup);//subtract the time taken for warmup
	if (verbose == 1) {//only rank 0 can have verbose == 0
		printf("%i %i %f %f %f\n",nprocs,n,interval_t,interval_p,interval_s);
	}

	Free_local_matrix(&local_A);
	Free_local_matrix(&local_B);
	Free_local_matrix(&local_C);

	return MPI_Finalize();
}

/*********************************************************/
void Setup_grid(GRID_INFO_T*  grid) {
	int old_rank;
	int dimensions[2];
	int wrap_around[2];
	int coordinates[2];
	int free_coords[2];

	MPI_Comm_size(MPI_COMM_WORLD, &(grid->p));//global grid info
	MPI_Comm_rank(MPI_COMM_WORLD, &old_rank);

	grid->q = (int) sqrt((double) grid->p);//assume nprocs is a prefect square
	dimensions[0] = dimensions[1] = grid->q;

	//circular shift in 2nd dimension, don't care about the first
	wrap_around[0] = wrap_around[1] = 1;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dimensions, wrap_around, 1, &(grid->comm));
	MPI_Comm_rank(grid->comm, &(grid->myrank));
	MPI_Cart_coords(grid->comm, grid->myrank, 2, coordinates);
	grid->my_row = coordinates[0];
	grid->my_col = coordinates[1];

	free_coords[0] = 0;//row communicators
	free_coords[1] = 1;
	MPI_Cart_sub(grid->comm, free_coords, &(grid->row_comm));

	free_coords[0] = 1;//column communicators
	free_coords[1] = 0;
	MPI_Cart_sub(grid->comm, free_coords, &(grid->col_comm));
}

/*********************************************************/
void Fox(int n,GRID_INFO_T* grid,LOCAL_MATRIX_T* local_A,LOCAL_MATRIX_T* local_B,LOCAL_MATRIX_T* local_C) {
	//local_C is output

	LOCAL_MATRIX_T* temp_A; //storage for the sub-matrix of A used during the current stage
	int stage;
	int bcast_root;
	int n_bar;// n/sqrt(p)
	int source;
	int dest;
	MPI_Status status;

	n_bar = n/grid->q;
	Set_to_zero(local_C);

	//addresses for circular shift of B
	source = (grid->my_row + 1) % grid->q;				// (l + 1) mod q
	dest = (grid->my_row + grid->q - 1) % grid->q;		// (l + q - 1) mod q

	temp_A = Local_matrix_allocate(n_bar);//space for A's bcast block

	for (stage = 0; stage < grid->q; stage++) {
		bcast_root = (grid->my_row + stage) % grid->q; // (l + stage) mod q --> (n)
		if (bcast_root == grid->my_col) {// if (n == k)
			MPI_Bcast(local_A, 1, local_matrix_mpi_t,bcast_root, grid->row_comm);	// Broadcasting of A[l,k] (sub-block)
			Local_matrix_multiply(local_A, local_B,local_C);	 // C[l,k] += (A[l,n] * B[n,k])
		}else {
			MPI_Bcast(temp_A, 1, local_matrix_mpi_t,bcast_root, grid->row_comm);
			Local_matrix_multiply(temp_A, local_B,local_C);
		}
		MPI_Sendrecv_replace(local_B, 1, local_matrix_mpi_t, dest, 0, source, 0, grid->col_comm, &status);
	}
} 

/*********************************************************/
LOCAL_MATRIX_T* Local_matrix_allocate(int local_order) {
	LOCAL_MATRIX_T* temp;

	temp = (LOCAL_MATRIX_T*) malloc(sizeof(LOCAL_MATRIX_T));
	return temp;
}

/*********************************************************/
void Free_local_matrix(LOCAL_MATRIX_T** local_A_ptr) {
	free(*local_A_ptr);
}

/*********************************************************/
void Generate_matrix(LOCAL_MATRIX_T*  local_A ,GRID_INFO_T* grid,int n) {
	int mat_row, mat_col;
	int grid_row, grid_col;
	int dest;
	int coords[2];
	float* temp;
	MPI_Status status;

	if (grid->myrank == 0) {
		//printf("%d\n", grid->myrank);
		temp = (float*) malloc(Order(local_A)*sizeof(float));
		for (mat_row = 0;  mat_row < n; mat_row++) {
			grid_row = mat_row/Order(local_A);
			coords[0] = grid_row;

			for (grid_col = 0; grid_col < grid->q; grid_col++) {
				coords[1] = grid_col;
				MPI_Cart_rank(grid->comm, coords, &dest);

				if (dest == 0) {
					for (mat_col = 0; mat_col < Order(local_A); mat_col++)
						*((local_A->entries)+mat_row*Order(local_A)+mat_col) = RandVal;
				} else {
					for(mat_col = 0; mat_col < Order(local_A); mat_col++)
						*(temp + mat_col) = RandVal;
					MPI_Send(temp, Order(local_A), MPI_FLOAT, dest, 0,grid->comm);
				}
			}
		}
		free(temp);
	} else {
		for (mat_row = 0; mat_row < Order(local_A); mat_row++)
			MPI_Recv(&Entry(local_A, mat_row, 0), Order(local_A),MPI_FLOAT, 0, 0, grid->comm, &status);
	}
}

/*********************************************************/
void Print_matrix(char* title,LOCAL_MATRIX_T* local_A,GRID_INFO_T* grid,int n) {//testing purposes only
	int		   mat_row, mat_col;
	int		   grid_row, grid_col;
	int		   source;
	int		   coords[2];
	float*	   temp;
	MPI_Status status;

	if (grid->myrank == 0) {
		temp = (float*) malloc(Order(local_A)*sizeof(float));
		printf("%s\n", title);
		for (mat_row = 0;  mat_row < n; mat_row++) {
			grid_row = mat_row/Order(local_A);
			coords[0] = grid_row;
			for (grid_col = 0; grid_col < grid->q; grid_col++) {
				coords[1] = grid_col;
				MPI_Cart_rank(grid->comm, coords, &source);
				if (source == 0) {
					for(mat_col = 0; mat_col < Order(local_A); mat_col++)
						printf("%6.1f ", Entry(local_A, mat_row, mat_col));
				} else {
					MPI_Recv(temp, Order(local_A), MPI_FLOAT, source, 0,grid->comm, &status);
					for(mat_col = 0; mat_col < Order(local_A); mat_col++)
						printf("%6.1f ", temp[mat_col]);
				}
			}printf("\n");
		}free(temp);
	} else {
		for (mat_row = 0; mat_row < Order(local_A); mat_row++)
			MPI_Send(&Entry(local_A, mat_row, 0), Order(local_A),MPI_FLOAT, 0, 0, grid->comm);
	}
}

/*********************************************************/
void Set_to_zero(LOCAL_MATRIX_T*  local_A) {
	int i, j;
	for (i = 0; i < Order(local_A); i++)
		for (j = 0; j < Order(local_A); j++)
			Entry(local_A,i,j) = 0.0;
}

/*********************************************************/
void Build_matrix_type(LOCAL_MATRIX_T*	local_A) {
	MPI_Datatype  temp_mpi_t;
	int			  block_lengths[2];
	MPI_Aint	  displacements[2];
	MPI_Datatype  typelist[2];
	MPI_Aint	  start_address;
	MPI_Aint	  address;

	MPI_Type_contiguous(Order(local_A)*Order(local_A),MPI_FLOAT, &temp_mpi_t);//array[0..n-1,0..n-1] of float

	block_lengths[0] = block_lengths[1] = 1;

	typelist[0] = MPI_INT;//2 elements: 1) int, 2) array[...] of float
	typelist[1] = temp_mpi_t;

	MPI_Address(local_A, &start_address);//adress of submatrix - start
	MPI_Address(&(local_A->n_bar), &address);//adress of Size of submatrix
	displacements[0] = address - start_address; //between n_bar and beginning (sometimes <>0 )

	MPI_Address(local_A->entries, &address);//adress of entries
	displacements[1] = address - start_address; //between entries and beginning

	MPI_Type_struct(2, block_lengths, displacements,typelist, &local_matrix_mpi_t);//new struct type
	MPI_Type_commit(&local_matrix_mpi_t);
}

/*********************************************************/
void Local_matrix_multiply(LOCAL_MATRIX_T* local_A,LOCAL_MATRIX_T* local_B,LOCAL_MATRIX_T* local_C) {
	int i, j, k;
	for (i = 0; i < Order(local_A); i++)
		for (j = 0; j < Order(local_A); j++)
			for (k = 0; k < Order(local_B); k++)
				Entry(local_C,i,j) += Entry(local_A,i,k)*Entry(local_B,k,j);
}
