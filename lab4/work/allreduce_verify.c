#include <stdio.h>
#include <mpi.h>

int my_allreduce(double* in,double* out,int count,MPI_Comm comm,int hi_lo){//if hi_lo==0, do high->low, else low->high
	int myrank, nprocs;
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
	int t=1,i;//loop vars
	MPI_Status status;
	double tmp[count];//to recieve messages
	
	char* hl=(hi_lo==0?"Hi->Lo":"Lo->Hi");
	
	for(t=0;t<count;t++){//start the out vector with the values in the in vector
		out[t]=in[t];//as we will add the other values we recieve to it
	}
	
	int step_count=0;
	if(hi_lo==0){
		for(t=nprocs/2;t>0;t/=2){//high to low
			step_count++;
			int other_proc=t^myrank;//bitwise XOR
			printf("%s:Rank %d swapping with rank %d on (mask is %d, step %d)\n",hl,myrank,other_proc,t,step_count);
		
			//send/recieve order so the lower rank sends first
			if(myrank<other_proc){//send then recieve
				MPI_Send(out,count,MPI_DOUBLE,other_proc,5,comm);//using out instead of in b/c out has the total thus far, in is only our values
				MPI_Recv(&tmp,count,MPI_DOUBLE,other_proc,5,comm,&status);
			}else{//recieve then send
				MPI_Recv(&tmp,count,MPI_DOUBLE,other_proc,5,comm,&status);
				MPI_Send(out,count,MPI_DOUBLE,other_proc,5,comm);
			}
		
			//add what we recieved to the out vector
			for(i=0;i<count;i++){
				out[i]+=tmp[i];
			}
		}
	}else{
		for(t=1;t<nprocs;t*=2){//low to high
			step_count++;
			int other_proc=t^myrank;//bitwise XOR
			printf("%s:Rank %d swapping with rank %d on (mask is %d, step %d)\n",hl,myrank,other_proc,t,step_count);
		
			//send/recieve order so the lower rank sends first
			if(myrank<other_proc){//send then recieve
				MPI_Send(out,count,MPI_DOUBLE,other_proc,5,comm);//using out instead of in b/c out has the total thus far, in is only our values
				MPI_Recv(&tmp,count,MPI_DOUBLE,other_proc,5,comm,&status);
			}else{//recieve then send
				MPI_Recv(&tmp,count,MPI_DOUBLE,other_proc,5,comm,&status);
				MPI_Send(out,count,MPI_DOUBLE,other_proc,5,comm);
			}
		
			//add what we recieved to the out vector
			for(i=0;i<count;i++){
				out[i]+=tmp[i];
			}
		}
	}
}

int main(int argc, char** argv) {
	int myrank, nprocs;
	int t=0;//loop var
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
	int data_length=2;//number of doubles to use
	if(argc>1){
		data_length=atoi(argv[1]);
		if(myrank==0){//only complain once
			printf("Using %s for data length\n",argv[1]);
			if(data_length<1){//we need at least 1 double to pass
				printf("Invalid data length: %d Please use a value greater than 0\n",data_length);
				exit(1);
			}
		}
	}
	
	//set up data to be allreduce'd
	double data[data_length];//this proc's data we send into allreduce
	double rec_buf[data_length];//the buffer allreduce will write to
	double rec_buf2[data_length];//the buffer allreduce will write to
	double rec_buf3[data_length];//the buffer allreduce will write to
	for(t=0;t<data_length;t++){
		data[t]=myrank;//set the whole array to the current process' rank
	}

	my_allreduce(&data,&rec_buf,data_length,MPI_COMM_WORLD,0);
	MPI_Barrier(MPI_COMM_WORLD);
	my_allreduce(&data,&rec_buf2,data_length,MPI_COMM_WORLD,1);
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Allreduce(&data,&rec_buf3,data_length,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);//in buffer,out buffer,count,datatype,operation,communicator
		
	//print verification here
	if(myrank==0){
		printf("Hi-Lo result vector: ");
		for(t=0;t<data_length;t++){
			printf("%f ",rec_buf[t]);
		}printf("\n");
		
		printf("Lo-Hi result vector: ");
		for(t=0;t<data_length;t++){
			printf("%f ",rec_buf2[t]);
		}printf("\n");
		
		printf("MPI's result vector: ");
		for(t=0;t<data_length;t++){
			printf("%f ",rec_buf3[t]);
		}printf("\n");
	}
	
	MPI_Finalize();
	return 0;
}
