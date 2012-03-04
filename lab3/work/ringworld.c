#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int myrank, nprocs;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    //printf("Hello from processor %d of %d\n", myrank, nprocs);
    
    int send_to,rec_from,message,tag;
    message=42;//doesn't matter what we start off sending
    tag=3;//anything will work
    send_to=(myrank+1)%nprocs;
    rec_from=myrank-1;
    if(rec_from<0){//our rank is zero, so we need to recieve from nprocs-1
    	rec_from=nprocs-1;//would % do this?
    }
    
    MPI_Status status;
    
    //do the sends & recieves
    if(myrank==0){//we want proc 0 to send first to start the chain
    	printf("process %d now sending message %d to process %d with tag %d\n",myrank,message,send_to,tag);
    	MPI_Send(&message,1,MPI_INT,send_to,tag,MPI_COMM_WORLD);
    }
    printf("process %d now recieving from process %d with tag %d\n",myrank,rec_from,tag);
    MPI_Recv(&message,1,MPI_INT,rec_from,tag,MPI_COMM_WORLD,&status);
    
    if(myrank!=0){//if we arent proc 0, we want to pass the message along (but it should stop at 0)
    	printf("process %d now sending message %d to process %d with tag %d\n",myrank,message,send_to,tag);
    	MPI_Send(&message,1,MPI_INT,send_to,tag,MPI_COMM_WORLD);
    }
    

    MPI_Finalize();
    return 0;
}
