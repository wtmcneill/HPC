#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include "hdf5.h"

namespace hdf5
{

void read(const std::string &filename, std::vector<int> &data)
{
	hid_t file_id, dataset_id, space_id; 
	herr_t status;
	
	file_id = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
	dataset_id = H5Dopen(file_id, "DATASET", H5P_DEFAULT);
	space_id = H5Dget_space(dataset_id);

	int length = H5Sget_simple_extent_npoints(space_id);
	int  * image = new int[length];
	status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, image);

	// Copy back to vector
	data.resize(length);
	for(int i=0; i<length; ++i){
		data[i] = image[i];
	}
	delete [] image;
		
	status = H5Sclose(space_id);
	status = H5Dclose(dataset_id);
	status = H5Fclose(file_id);
}

void write(const std::string &filename, std::vector<int> &data)
{
	// HDF5 handles
	hid_t file_id, dataset_id, space_id, property_id; 
    herr_t status;

	hsize_t  dims[1] = {data.size()};
    
    //Create a new file using the default properties.
    file_id = H5Fcreate (filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    //Create dataspace.  Setting maximum size to NULL sets the maximum
    //size to be the current size.
    space_id = H5Screate_simple (1, dims, NULL);

    //Create the dataset creation property list, set the layout to compact.
    property_id = H5Pcreate (H5P_DATASET_CREATE);
    status = H5Pset_layout (property_id, H5D_COMPACT);

    // Create the dataset. 
    dataset_id = H5Dcreate (file_id, "DATASET", H5T_STD_I32LE, space_id, H5P_DEFAULT, property_id, H5P_DEFAULT);
   
    //Write the data to the dataset.
    status = H5Dwrite (dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data[0]);

   	status = H5Sclose(space_id);
	status = H5Dclose(dataset_id);
	status = H5Fclose(file_id);
	status = H5Pclose(property_id);
}

};


int main(int argc, char ** argv)
{

	std::vector<int> values(30,10);
	std::fill (values.begin(),values.begin()+10,5); 
	
	std::string filename = "data-1d.hdf5";
	
	hdf5::write(filename,values);
	
	std::vector<int> results;
	hdf5::read(filename, results);
	
	std::cout << "results" << std::endl;
	for(int i=0; i<results.size(); ++i)
	{
		std::cout << results[i] << " ";
	}
	std::cout << std::endl;
	
    return 0;
}

