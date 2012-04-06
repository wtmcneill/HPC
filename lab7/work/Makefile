CC      =  g++
OPT 	  =  -fopenmp

OPTIONS =  $(OPT)
EXEC   	= omp_MatVecMultiply

OBJS   	= main.o matvecMult.o getArgs.o 

INCL   	= proto.h

CFLAGS 	= $(OPTIONS) 

$(EXEC): $(OBJS) 
	$(CC) $(OPT) $(OBJS)   -o $(EXEC)  

$(OBJS): $(INCL) 

clean:
	rm -f $(OBJS) $(EXEC)
