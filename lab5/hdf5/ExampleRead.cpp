#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <hdf5.h>
#include <mpi.h>

// const int dim1 = 10;
// const int dim2 = 10;

void write_file(const std::string & filename, const int dim1, const int dim2)
{
	hid_t file_id, dataset_id, space_id, property_id; 
    herr_t status;

	hsize_t  dims[2] = {dim1, dim2};
    double * image = new double[dim1*dim2];
    for (int i=0; i<dim1; i++)
        for (int j=0; j<dim2; j++)
            image[i*dim2 + j] = i*dim2 + j + 1;
       
    //Create a new file using the default properties.
    file_id = H5Fcreate (filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

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
	
	delete [] image;
}

void write_chunked_file(const std::string & filename, 
						const int dim1, const int dim2,
						const int chunk1, const int chunk2)
{   
    herr_t  status;
    hid_t file = H5Fcreate (filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    
    double * image = new double[dim1*dim2];
    for (int i=0; i<dim1; i++)
        for (int j=0; j<dim2; j++)
            image[i*dim2 + j] = i*dim2 + j + 1;
	
    hsize_t dims[2] ={dim1,dim2};
    hsize_t chunk[2] = {chunk1, chunk2};
    
    hid_t space = H5Screate_simple (2, dims, NULL);
    hid_t dcpl = H5Pcreate (H5P_DATASET_CREATE);
    status = H5Pset_chunk (dcpl, 2, chunk);
    hid_t dset = H5Dcreate (file, "DATASET", H5T_STD_I32LE, space, H5P_DEFAULT, dcpl, H5P_DEFAULT);

    hsize_t start[2] = {0,0};
    hsize_t stride[2] = {1,1};
    hsize_t count[2] = {1,dim2};	
    hsize_t block[2] = {dim1,1};	
	
    status = H5Sselect_hyperslab (space, H5S_SELECT_SET, start, stride, count, block);
    status = H5Dwrite (dset, H5T_NATIVE_DOUBLE, H5S_ALL, space, H5P_DEFAULT, image);

    delete [] image;
    status = H5Pclose (dcpl);
    status = H5Dclose (dset);
    status = H5Sclose (space);
    status = H5Fclose (file);
}

int read_partial(const std::string & filename, int num_rows)
{
    herr_t  status;
    hid_t file = H5Fopen (filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    hid_t dset = H5Dopen (file, "DATASET", H5P_DEFAULT);
    
    hid_t dcpl = H5Dget_create_plist (dset);
    H5D_layout_t layout = H5Pget_layout (dcpl);
   
     hid_t space = H5Dget_space (dset);
 	 hsize_t dims[2], mdims[2];
 	 status = H5Sget_simple_extent_dims(space,dims,mdims);
	 
	 int dim1 = dims[0];
	 int dim2 = dims[1];
	 
	 if( dim1%num_rows != 0)
	 {
		 //std::cout << "dim1%num_rows != 0" << std::endl;
		 return 0;
	 }
	 
	 const int length = dim2*num_rows;
	 double * image = new double[length];
     for(int r=0; r<dim1; r+=num_rows)
     {
	    // Select from the file
        hsize_t offset[2] = {r,0};
        hsize_t count[2] = {num_rows,dim2};
		status = H5Sselect_hyperslab (space, H5S_SELECT_SET, offset, NULL, count, NULL);
        
		// Create memory space
		hsize_t dim_out[2] = {num_rows,dim2};
        hsize_t offset_out[2] = {0,0};
        hsize_t count_out[2] = {num_rows,dim2};
		
		hid_t memspace = H5Screate_simple(2,dim_out,NULL);   
		status = H5Sselect_hyperslab(memspace, H5S_SELECT_SET, offset_out, NULL, count_out, NULL);
		
		status = H5Dread (dset, H5T_NATIVE_DOUBLE, memspace, space, H5P_DEFAULT, image);
		
		// std::cout << "r = " << r << std::endl;
		// 	for(int i=0; i<length; ++i)
		// 		std::cout << image[i] << " ";
		// 	std::cout << std::endl;
		
     }
	 delete [] image;
        
    status = H5Pclose (dcpl);
    status = H5Dclose (dset);
    status = H5Sclose (space);
    status = H5Fclose (file);
	
	return 1;
}

void read_full(const std::string & filename)
{
    herr_t  status;
    hid_t file = H5Fopen (filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    hid_t dset = H5Dopen (file, "DATASET", H5P_DEFAULT);
    hid_t space = H5Dget_space (dset);
	hsize_t dims[2], mdims[2];
	status = H5Sget_simple_extent_dims(space,dims,mdims);
   
 	int dim1 = dims[0];
 	int dim2 = dims[1];
		
    double * image = new double[dim1*dim2];

    hid_t dcpl = H5Dget_create_plist (dset);
    H5D_layout_t layout = H5Pget_layout (dcpl);
    switch (layout) 
   {
        case H5D_COMPACT:
               std::cout << std::setw(20) << "H5D_COMPACT" ;
               break;
        case H5D_CONTIGUOUS:
              std::cout << std::setw(20) << "H5D_CONTIGUOUS";
               break;
        case H5D_CHUNKED:
               std::cout << std::setw(20) << "H5D_CHUNKED";
    }
    status = H5Dread (dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, image);
    
    delete [] image;
    status = H5Pclose (dcpl);
    status = H5Dclose (dset);
    status = H5Fclose (file);	
}

double print_time(double &start)
{
    double end_time = MPI_Wtime();
    double value = end_time - start;
    start = MPI_Wtime();
	return value;
}

void read_partial_row(const std::string &file_name, int num_rows)
{
	double start_time = MPI_Wtime();
	if( read_partial(file_name,num_rows) )
	{
		std::cout << std::setw(20) << num_rows<< std::setw(20);
		std::cout << print_time(start_time) << std::endl;
	}
}

void read_full_file(const std::string &file_name)
{
	std::cout << "filename = " << file_name << std::endl;
	double start_time = MPI_Wtime();
	read_full(file_name);	
	std::cout << std::setw(20);
	std::cout << print_time(start_time) << std::endl;
}

int main(int argc, char ** argv)
{
    MPI_Init(&argc, &argv);
		
	const int dim1 = atoi(argv[1]);
	const int dim2 = dim1;
	
	// Continuous
	std::vector<std::string> file_names;
	std::stringstream ss;
	std::string file_name;
	
	// Create files if needed...
	ss << "contiguous." << dim1 << ".hdf5";
	file_name = ss.str();
	file_names.push_back(file_name);
	write_file(file_name, dim1, dim2);
		
	ss.str("");
	ss << "chunked." << dim1 << ".hdf5";
	file_name = ss.str();
	file_names.push_back(file_name);
	write_chunked_file(file_name, dim1, dim2, dim1, 1);
	
	// Run the test...
	for(int f=0; f<file_names.size(); ++f)
	{
		read_full_file(file_names[f]);
		int r = dim1;
		while( r > 0)
		{
			read_partial_row(file_names[f], r); 
			r/=2;
		}
	}
	
    MPI_Finalize();
   
    return 0;
}
