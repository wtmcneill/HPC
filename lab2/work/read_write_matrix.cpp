#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include "hdf5.h"

namespace hdf5{
	std::string GetFileExtension(const std::string& FileName){
		if(FileName.find_last_of(".") != std::string::npos)
			return FileName.substr(FileName.find_last_of(".")+1);
		return "";
	}

void write(const std::string &filename, std::vector<int> &data)
{
	clock_t start,stop;
	start=clock();

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

	stop=clock();
	std::cout << "hdf5_write	" << (data[0]) << "	" << (stop-start) << std::endl;
}

void read(const std::string &filename, std::vector<int> &data)
{
	clock_t start,stop;
	start=clock();

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

	stop=clock();
	std::cout << "hdf5_read	" << (data[0]) << "	" << ((double)(stop-start))/CLOCKS_PER_SEC << "	" << CLOCKS_PER_SEC << std::endl;
}

};

namespace text{
	void read(const std::string &filename, std::vector<int> &data){//works
		clock_t start,stop;
		start=clock();

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

		stop=clock();
		std::cout << "text_read	" << (data[0]) << "	" << (stop-start) << std::endl;
	}
	
	void write(const std::string &filename, std::vector<int> &data){
		clock_t start,stop;
		start=clock();
		
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

		stop=clock();
		std::cout << "text_write	" << (data[0]) << "	" << (stop-start) << std::endl;
	}
}

int main(int argc, char ** argv){
	
	if (argc < 3){
		printf("USAGE: read_write_matrix input_file.ext output_file.ext\n");
		printf("where ext is either hdf5 or txt\n");
		return 0;
	}
	
	char * in = argv[1];
	char * out= argv[2];
	std::string in_str(in);
	std::string out_str(out);
	
	std::string in_ext= hdf5::GetFileExtension(in_str);
	std::string out_ext=hdf5::GetFileExtension(out_str);
	
	std::vector<int> values(32,10);
	
	if(in_ext.compare("txt")==0){
		text::read(in_str,values);
	}else if(in_ext.compare("hdf5")==0){
		hdf5::read(in_str,values);//hdf5 read
	}else{
		std::cout << in_ext << " files not supported!" << std::endl;
		return 1;
	}
	
	if(out_ext.compare("txt")==0){
		text::write(out_str,values);
	}else if(out_ext.compare("hdf5")==0){
		hdf5::write(out_str,values);//hdf5 write
	}else{
		std::cout << out_ext << " files not supported" << std::endl;
		return 1;
	}
	
	return 0;
}
