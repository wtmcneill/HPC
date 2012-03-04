#include <iostream>
#include <string>
#include <sstream>
#include <hdf5.h>

void write_file(const std::string & filename, double * image, int r, int c)
{
  hid_t file_id, dataset_id, space_id, property_id; 
  herr_t status;

  hsize_t  dims[2] = {r, c};
  file_id = H5Fcreate (filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

  space_id = H5Screate_simple (2, dims, NULL);
  property_id = H5Pcreate (H5P_DATASET_CREATE);
  status = H5Pset_layout (property_id, H5D_CONTIGUOUS);
  dataset_id = H5Dcreate (file_id, "DATASET", H5T_STD_I32LE, space_id, H5P_DEFAULT, property_id, H5P_DEFAULT);
  status = H5Dwrite (dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, image);

  status = H5Sclose(space_id);
  status = H5Dclose(dataset_id);
  status = H5Fclose(file_id);
  status = H5Pclose(property_id);
}

int main(int argc, char ** argv)
{
	if( argc < 1 )
		exit;

	int size = atoi(argv[1]);
	
	double * matrix = new double[size*size];
	double * vector = new double[size];
	
  for (int i=0; i<size; i++)
    for (int j=0; j<size; j++)
        matrix[i*size + j] = (i*size + j + 1);
	
	for(int i=0; i<size; ++i)
		vector[i] = (i+1);
	
	std::stringstream ss;
	ss << "matrix" << size << ".hdf5";
	std::string m_file_name = ss.str();
	ss.clear();
	ss << "vector" << size << ".hdf5";
	std::string v_file_name = ss.str();
	
	write_file(m_file_name, matrix, size, size);
	write_file(v_file_name, vector, size, 1);
	
	delete [] vector;
	delete [] matrix;
  return 0;
}
