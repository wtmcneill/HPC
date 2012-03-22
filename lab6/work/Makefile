#HDF5_INCLUDE=-I/curc/tools/free/redhat_5_x86_64/hdf5-1.8.7/include
#HDF5_LIB_DIR=-L/curc/tools/free/redhat_5_x86_64/hdf5-1.8.7/lib
HDF5_INCLUDE=-I/home/will/hdf5-1.8.8-linux-x86_64-shared/include
HDF5_LIB_DIR=-L/home/will/hdf5-1.8.8-linux-x86_64-shared/lib
HDF5_LIBS=-lhdf5


INCLUDE=$(HDF5_INCLUDE)
LIB_DIR=$(HDF5_LIB_DIR)
LIBS=$(HDF5_LIBS)

all:
	#make multiply
	#make noIO_multiply
	make fox
	make canon

fox: fox_multiply.c
	mpicc fox_multiply.c -o fox_multiply

canon: canons_multiply.c
	mpicc canons_multiply.c -o canons_multiply

multiply: multiply.c
	mpicc multiply.c $(INCLUDE) $(LIB_DIR) $(LIBS) -o multiply

noIO_multiply: noIO_multiply.c
	mpicc noIO_multiply.c -o noIO_multiply

clean:
	rm -f multiply
	rm -f noIO_multiply
	rm -f fox_multiply
	rm -f canons_multiply
