#include <iostream>
#include <algorithm>
#include <iterator>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>
#include <limits.h>

// y = x
double f(const double &x)
{
	return x;
}

double trapezoidal(const double &a, const double &b, const  unsigned long int &n)
{
	int rank, num_ranks;
	MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	// Linear combination variables
	const unsigned long int A = n%static_cast< unsigned long int>(num_ranks);
	const unsigned long int B = num_ranks-A;

	unsigned long int local_n = n/static_cast< unsigned long int>(num_ranks);//how many steps does this proc do?
	if( rank < A)
		local_n += 1;
	
	double step = (b - a)/static_cast<double>(n);//step size between each evaluation
	
	double start_n = local_n*rank;
	if( rank >= A)
		start_n = A*(local_n+1) + (rank-A)*local_n;
	start_n *= step;
		
	double integral = (f(start_n) + f(start_n +local_n*step - step));
	
	unsigned long int i;
	#pragma omp parallel for private(i) reduction(+:integral)
	for(i=1; i<local_n-1; ++i)
	{
		double x = start_n +i*step;
		integral = f(x);
	}
	integral *= step;

	double total_integral = 0.0;
	MPI_Reduce(&integral, &total_integral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	
	return total_integral;
}


int main(int argc, char ** argv)
{
	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	// Arguments...
	const unsigned long int precision = 4e9;
	const double a = 0.0;
	const double b = 10.0;
	
	double start_time = MPI_Wtime();
	
	// compute integral
	double integral = trapezoidal(a,b,precision);	
	
	double end_time = MPI_Wtime();
		
	if( rank == 0)
		std::cout <<  size << " " << integral << " " << end_time - start_time << std::endl << std::endl;
	
	MPI_Finalize();
	
	return 1;
}
