#include <hdf5.h>
#include <mpi.h>
//#include <iostream>
//#include <string>
//#include <sstream>
//using namespace std;

void write_file(char* filename, const int dim1, const int dim2, double* image)
{
	hid_t file_id, dataset_id, space_id, property_id;
	herr_t status;

	hsize_t  dims[2] = {dim1, dim2};

	//Create a new file using the default properties.
	file_id = H5Fcreate (filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	//Create dataspace.  Setting maximum size to NULL sets the maximum
	//size to be the current size.
	space_id = H5Screate_simple (2, dims, NULL);

	//Create the dataset creation property list, set the layout to compact.
	property_id = H5Pcreate (H5P_DATASET_CREATE);
	status = H5Pset_layout (property_id, H5D_CONTIGUOUS);

	// Create the dataset. 
	dataset_id = H5Dcreate (file_id, "DATASET", H5T_STD_I32LE, space_id, H5P_DEFAULT, property_id, H5P_DEFAULT);

	//Write the data to the dataset.
	status = H5Dwrite (dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, image);

	status = H5Sclose(space_id);
	status = H5Dclose(dataset_id);
	status = H5Fclose(file_id);
	status = H5Pclose(property_id);
}

double* read_full(char* filename, int* dim1,int* dim2)
{
    herr_t  status;
    hid_t file = H5Fopen (filename, H5F_ACC_RDONLY, H5P_DEFAULT);
    hid_t dset = H5Dopen (file, "DATASET", H5P_DEFAULT);
    hid_t space = H5Dget_space (dset);
	hsize_t dims[2], mdims[2];
	status = H5Sget_simple_extent_dims(space,dims,mdims);
   
 	*dim1 = dims[0];
 	*dim2 = dims[1];
		
    double* image=(double*)malloc(dims[0]*dims[1]*sizeof(double));

    hid_t dcpl = H5Dget_create_plist (dset);
    H5D_layout_t layout = H5Pget_layout (dcpl);

    status = H5Dread (dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, image);
    
    status = H5Pclose (dcpl);
    status = H5Dclose (dset);
    status = H5Fclose (file);
    
    return image;
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
	
	int root=0;//which process is the root
	int size,dim2,vd1,vd2;
	double* matrix;
	double* vector;
	
	if(partial==0){
		if(myrank==root){//if we arent doing it partially, only root should read
			matrix=read_full(matrix_file,&size,&dim2);
			if(size!=dim2){printf("ERROR: Non-square matrix! Funny things will happen now!");}
		}
	}else{
		//TODO: partial read
	}
	if(myrank==root){vector=read_full(vector_file,&vd1,&vd2);}
	
	int rows_per_process=size/nprocs;//also vector elements per process
	int elements_per_process=size*size/nprocs;
	double output[size*size];
	double my_rows[elements_per_process];
	double my_out[elements_per_process];
	
	if(partial==0){//scatter the matrix if only root read it
		MPI_Scatter(matrix,elements_per_process,MPI_DOUBLE,my_rows,elements_per_process,MPI_DOUBLE,root,MPI_COMM_WORLD);
	}
	//broadcast the vector
	MPI_Bcast(vector,size,MPI_DOUBLE,root,MPI_COMM_WORLD);
	
	do_multiply(my_rows,vector,my_out,rows_per_process,elements_per_process);
	
	//gather
	MPI_Gather(my_out,elements_per_process,MPI_DOUBLE,output,elements_per_process,MPI_DOUBLE,root,MPI_COMM_WORLD);
	
	if(myrank==root){
		printf("Final matrix:\n");
		print_matrix(output,size,size);
		if(strcmp(out_file, "")!=0){//if we have an output file
			write_file(out_file,size,size,output);
		}
	}
	
	MPI_Finalize();
	return 0;
}
