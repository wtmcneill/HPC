CC=/usr/bin/g++

HDF5_INCLUDE=-I/curc/tools/free/redhat_5_x86_64/hdf5-1.8.7/include
HDF5_LIB_DIR=-L/curc/tools/free/redhat_5_x86_64/hdf5-1.8.7/lib
HDF5_LIBS=-lhdf5

MKL_INCLUDE=-I/curc/tools/nonfree/redhat_5_x86_64/ics_2012.0.032/composer_xe_2011_sp1.6.233/mkl/include
MKL_LIB_DIR=-L/curc/tools/nonfree/redhat_5_x86_64/ics_2012.0.032/composer_xe_2011_sp1.6.233/mkl/lib/intel64
MKL_LIBS=-lmkl_intel_lp64 -lmkl_sequential -lmkl_core


INCLUDE=$(HDF5_INCLUDE)
LIB_DIR=$(HDF5_LIB_DIR)
LIBS=$(HDF5_LIBS)

all:
	make read_write_matrix
	make naive_multiply
	make block_multiply
	make intel_multiply

read_write_matrix:
	$(CC) read_write_matrix.cpp $(INCLUDE) $(LIB_DIR) $(LIBS) -o read_write_matrix

naive_multiply:
	$(CC) naive_multiply.cpp $(INCLUDE) $(LIB_DIR) $(LIBS) -o naive_multiply

block_multiply:
	$(CC) block_multiply.cpp $(INCLUDE) $(LIB_DIR) $(LIBS) -o block_multiply

intel_multiply:
	$(CC) intel_multiply.cpp $(INCLUDE) $(LIB_DIR) $(LIBS) $(MKL_INCLUDE) $(MKL_LIBS_DIR) $(MKL_LIBS) -o intel_multiply

clean:
	rm read_write_matrix
	rm naive_multiply
	rm block_multiply
	rm intel_multiply

