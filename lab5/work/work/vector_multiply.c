//#include <hdf5.h>
#include <mpi.h>
//#include <iostream>
//#include <string>
//#include <sstream>
//using namespace std;

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

void do_multiply(double* mat,double* vec,double* out,int rows,int elements){
	int r,c;
	int epr=elements/rows;//elements per row
	int ind;//use to store indicies
	for(r=0;r<rows;r++){
		for(c=0;c<epr;c++){
			ind=r*epr+c;
			out[ind]=mat[ind]*vec[c];
		}
	}
}

int main(int argc, char ** argv)
{
	int nprocs,myrank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
	char* matrix_file="";
	char* vector_file="";
	char* out_file="";
	int verb=0,partial=0;
	
	int i=1;
	for(i=1;i<argc;i++){//step through arguments
		if (strcmp(argv[i], "-m") == 0){
			matrix_file=argv[i+1];
		}else if (strcmp(argv[i], "-v") == 0){
			vector_file=argv[i+1];
		}else if (strcmp(argv[i], "-o") == 0){
			out_file=argv[i+1];
		}else if (strcmp(argv[i], "-verbose") == 0){
			verb=1;
		}else if (strcmp(argv[i], "-partial") == 0){
			partial=1;
		}
	}
	
	int size=8;
	int root=0;
	int rows_per_process=size/nprocs;//also vector elements per process
	int elements_per_process=size*size/nprocs;
	double matrix[size*size];
	double vector[size];
	double output[size*size];
	double my_rows[elements_per_process];
	//double my_vec[rows_per_process];
	double my_out[elements_per_process];
	
	//make some sample data before we have code in place to read the file
	if(myrank==root){
		int i,j;
		for (i=0; i<size; i++)
			for (j=0; j<size; j++){matrix[i*size + j] = (i*size + j + 1);}
		
		for(i=0; i<size; ++i){vector[i] = (i+1);}
		//print_matrix(&matrix,size,size);
		//print_row(&vector,size);
	}
	
	//scatter the matrix
	MPI_Scatter(matrix,elements_per_process,MPI_DOUBLE,my_rows,elements_per_process,MPI_DOUBLE,root,MPI_COMM_WORLD);
	//broadcast the vector
	MPI_Bcast(vector,size,MPI_DOUBLE,root,MPI_COMM_WORLD);
	
	/*if(myrank==root){
		printf("Proc %i got matrix data:\n",myrank);
		print_matrix(my_rows,rows_per_process,size);
		
		printf("Proc %i got vector data: ",myrank);
		print_row(vector,size);
	}*/
	
	do_multiply(my_rows,vector,my_out,rows_per_process,elements_per_process);
	
	/*if(myrank==root){
		printf("Proc %i multiplied & got:\n",myrank);
		print_matrix(my_out,rows_per_process,size);
	}*/
	
	//gather
	MPI_Gather(my_out,elements_per_process,MPI_DOUBLE,output,elements_per_process,MPI_DOUBLE,root,MPI_COMM_WORLD);
	
	if(myrank==root){
		printf("Final matrix:\n");
		print_matrix(output,size,size);
	}
	
	MPI_Finalize();
	return 0;
}
