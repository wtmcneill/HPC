all: allreduce allreduce_verify allreduce_timer

allreduce: allreduce.c
	mpicc allreduce.c -o allreduce

allreduce_verify: allreduce_verify.c
	mpicc allreduce_verify.c -o allreduce_verify
	
allreduce_timer: allreduce_timer.c
	mpicc allreduce_timer.c -o allreduce_timer

clean:
	rm allreduce
	rm allreduce_verify
	rm allreduce_timer
