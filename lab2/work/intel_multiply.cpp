#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include "hdf5.h"
extern "C"{
	#include <mkl_cblas.h>
	//#include <gsl/gsl_cblas.h>
}

//prototypes
int get_index(int n,int x,int y);
std::string GetFileExtension(const std::string& FileName);
void multiply(std::vector<int> &in1,std::vector<int> &in2,std::vector<int> &out);
void text_read(const std::string &filename, std::vector<int> &data);
void text_write(const std::string &filename, std::vector<int> &data);
void hdf5_read(const std::string &filename, std::vector<int> &data);
void hdf5_write(const std::string &filename, std::vector<int> &data);
void print_matrix(std::vector<double> &mat);


void hdf5_read(const std::string &filename, std::vector<int> &data){
	hid_t file_id, dataset_id, space_id;
         herr_t status;
 
         file_id = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
         dataset_id = H5Dopen(file_id, "DATASET", H5P_DEFAULT);
         space_id = H5Dget_space(dataset_id);
 
         hsize_t dims[2]={0,0};
         status = H5Sget_simple_extent_dims(space_id,dims,NULL);
         int length=dims[0]*dims[1];
         int* image=new int[dims[0]*dims[1]];//temporary 2d array
         status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, image);
 
         // Copy back to vector
         data.resize(length+2);
         data[0]=dims[0];//copy over the dimensions first
         data[1]=dims[1];
         for(int i=0; i<length; ++i){
                 data[i+2] = image[i];
         }
         delete [] image;
 
         status = H5Sclose(space_id);
         status = H5Dclose(dataset_id);
         status = H5Fclose(file_id);
}

void hdf5_write(const std::string &filename, std::vector<int> &data){

    // HDF5 handles
    hid_t file_id, dataset_id, space_id, property_id;
    herr_t status;

    hsize_t  dims[2] = {data[0],data[1]};

    //Create a new file using the default properties.
    file_id = H5Fcreate (filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    //Create dataspace.  Setting maximum size to NULL sets the maximum
    //size to be the current size.
    space_id = H5Screate_simple (2, dims, NULL);

    //Create the dataset creation property list, set the layout to compact.
    property_id = H5Pcreate (H5P_DATASET_CREATE);
    status = H5Pset_layout (property_id, H5D_COMPACT);

    // Create the dataset. 
    dataset_id = H5Dcreate (file_id, "DATASET", H5T_STD_I32LE, space_id, H5P_DEFAULT, property_id, H5P_DEFAULT);

    //Write the data to the dataset.
    status = H5Dwrite (dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data[2]);

    status = H5Sclose(space_id);
    status = H5Dclose(dataset_id);
    status = H5Fclose(file_id);
    status = H5Pclose(property_id);


}

void text_read(const std::string &filename, std::vector<int> &data){//works
	//std::string tmp;
	std::ifstream infile;
	infile.open (filename.c_str());
	data.clear();//take everything out of the vector before we put things in
	int tmp=-1;
	infile >> tmp;
	data.push_back(tmp);//these are for the dimensions
	infile >> tmp;
	data.push_back(tmp);
	
	for(int i=0;i<data[0];i++){
		for(int j=0;j<data[1];j++){
			infile >> tmp;
			data.push_back(tmp);
			//std::cout << tmp << " ";
		}
	}
	
	infile.close();
}

void text_write(const std::string &filename, std::vector<int> &data){
	
	std::ofstream myfile;
	myfile.open (filename.c_str());//needs a char* instead of string
	//myfile << "Writing this to a file.\n";
	int n=data[0];
	int m=data[1];
	myfile << n << " " << m << "\n";
	for(int i=0;i<n;i++){
		for(int j=0;j<m;j++){
			myfile << data[i*m+j+2] << " ";//the extra +2 is because the first 2 values are array dimensions
			//std::cout << data[i*m+j+2] << " ";
		}
		myfile << "\n";
	}
	myfile.close();
}

//do intel CBLAS matrix multiply
void multiply(std::vector<int> &in1,std::vector<int> &in2,std::vector<int> &out){
	int n1=in1[0];
	int n2=in2[0];
	int m1=in1[1];
	int m2=in2[1];
	if(n2!=m1){std::cout<<"Inner dimensions don't match!"<<std::endl;return;}
	int n_out=n1;
	int m_out=m2;
	
	out.clear();//clear the vector we are writing into
	out.resize(n_out*m_out+2,0);//make the vector as big as we will need
	out[0]=n_out;//write the dimensions to the destination vector
	out[1]=m_out;
	
	int M=n1;//rows in A & C
	int N=m2;//cols in B & C
	int K=n2;//cols in A & rows in C - m1 is the same value

	std::vector<double> in1f(in1.begin(),in1.end());//convert to a double
	double *A=new double[n1*m1];//make an array we will use to pass to CBLAS
	A=&in1f[2];//define A to point to the second element in the vector
	std::vector<double> in2f(in2.begin(),in2.end());
	double *B=new double[n2*m2];
	B=&in2f[2];//define B
	std::vector<double> outf(out.begin(),out.end());
	double *C=new double[n_out*m_out];
	C=&outf[2];//define C
	
	int lda=n1;//these are the first dimensions of A,B,&C
	int ldb=n2;
	int ldc=n_out;

	/*//printing things for debugging
	std::cout << "M:" << M << std::endl; 
	std::cout << "N:" << N << std::endl; 
	std::cout << "K:" << K << std::endl; 
	std::cout << "A:" << A << std::endl;
	print_matrix(in1f);
	std::cout << "lda:" << lda << std::endl; 
	std::cout << "B:" << B << std::endl;
	print_matrix(in2f);
	std::cout << "ldb:" << ldb << std::endl; 
	std::cout << "C:" << C << std::endl;
	print_matrix(outf);
	std::cout << "ldc:" << ldc << std::endl; 
	*/

	cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,M,N,K,1.0,A,ldb,B,lda,1.0,C,ldc);
	
	//prints for debugging
	//std::cout << "new C:" << std::endl;
	//print_matrix(outf);
	
	//put C into the vector out
	//std::vector<int> tmp(outf.begin(),outf.end());
	out.assign(outf.begin(),outf.end());
}

//helper function for debugging
void print_matrix(std::vector<double> &mat){
	int n=(int)mat[0];
	int m=(int)mat[1];
	for(int i=0;i<n;i++){
		for(int j=0;j<m;j++){
			std::cout << mat[get_index(n,j,i)] << " ";
		}std::cout << std::endl;
	}
}

//the indexing into the vector is funny (the first two elements are the dimensions), so a helper function
int get_index(int n,int x,int y){
	return y*n+x+2;
}

std::string GetFileExtension(const std::string& FileName){
	if(FileName.find_last_of(".") != std::string::npos)
		return FileName.substr(FileName.find_last_of(".")+1);
	return "";
}

int main(int argc, char ** argv){
	
	if (argc < 4){
		printf("USAGE: naive_multiply input_file_1.ext input_file_2.ext output_file.ext\n");
		printf("where ext is either hdf5 or txt\n");
		return 0;
	}
	
	std::string in1_str(argv[1]);//convert char* -> string
	std::string in2_str(argv[2]);
	std::string out_str(argv[3]);
	
	std::string in1_ext= GetFileExtension(in1_str);//extract file extention
	std::string in2_ext= GetFileExtension(in2_str);
	std::string out_ext= GetFileExtension(out_str);
	
	std::vector<int> in1(0,0);//doesn't matter what we initialize to, read will clear it
	std::vector<int> in2(0,0);
	std::vector<int> out(0,0);
	
	//read first input file
	if(in1_ext.compare("txt")==0){
		text_read(in1_str,in1);
	}else{// if(in1_ext.compare("hdf5")==0){
		hdf5_read(in1_str,in1);//hdf5 read
	}//else{
	//	std::cout << in1_ext << " files not supported!!" << std::endl;
	//	return 1;
	//}
	
	//read second input file
	if(in2_ext.compare("txt")==0){
		text_read(in2_str,in2);
	}else{// if(out_ext.compare("hdf5")==0){
		hdf5_read(in2_str,in2);//hdf5 read
	}//else{
	//	std::cout << in2_ext << " files not supported!" << std::endl;
	//	return 1;
	//}
	
	clock_t start=clock();
	//do the matrix multiply
	multiply(in1,in2,out);
	std::cout << (in1[0]) << "	" << (clock()-start) << std::endl;
	
	//write the output file
	if(out_ext.compare("txt")==0){
		text_write(out_str,out);
	}else if(out_ext.compare("hdf5")==0){
		hdf5_write(out_str,out);//hdf5 write
	}else{
		std::cout << out_ext << " files not supported" << std::endl;
		return 1;
	}
	
	return 0;
}