CXX=/curc/tools/nonfree/redhat_5_x86_64/ics_2012.0.032/composer_xe_2011_sp1.6.233/bin/intel64/icpc


MKL_INCLUDE=-I/curc/tools/nonfree/redhat_5_x86_64/ics_2012.0.032/composer_xe_2011_sp1.6.233/mkl/include
MKL_LIB_DIR=-L/curc/tools/nonfree/redhat_5_x86_64/ics_2012.0.032/composer_xe_2011_sp1.6.233/mkl/lib/intel64
MKL_LIBS=-lmkl_intel_lp64 -lmkl_sequential -lmkl_core

INCLUDE=$(MKL_INCLUDE)
LIB_DIR=$(MKL_LIB_DIR)
LIBS=$(MKL_LIBS)

all:
	rm -f example_mkl
	make example_mkl

example_mkl:
	$(CXX) example_mkl.cpp $(INCLUDE) $(LIB_DIR) $(LIBS) -o example_mkl



