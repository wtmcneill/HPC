all: simpsons_parallel simpsons_serial ringworld

simpsons_parallel: simpsons_parallel.c
	mpicc simpsons_parallel.c -std=c99 -o simpsons_parallel

simpsons_serial: simpsons_serial.c
	g++ -Wall simpsons_serial.c -o simpsons_serial

ringworld: ringworld.c
	mpicc ringworld.c -o ringworld

clean:
	rm simpsons_serial
	rm simpsons_parallel
	rm ringworld
