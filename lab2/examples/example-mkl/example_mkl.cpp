#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

#include "mkl_cblas.h"


void print(double *d1, const int n1)
{
	for(int i=0; i<n1; ++i)
		std::cout << d1[i] << " ";
	std::cout << std::endl;

}

int main(int argc, char ** argv)
{

	const int N = 5;
	
	double * vector_1 = new double[N];
	for(int i=0; i<N; ++i)
		vector_1[i] = 10;
	
	double * vector_2 = new double[N];
	for(int i=0; i<N; ++i)
		vector_2[i] = 5;

	// Print
	print(vector_1, N);
	print(vector_2, N);
	
	// The number of elements in the vectors
	// 1 = stride (use every element)
	double value = cblas_ddot(N, vector_1, 1, vector_2, 1);
	
	std::cout << value << std::endl;
	
	
	delete [] vector_1;
	delete [] vector_2;
	
    return 0;
}
