CC=mpicc
CFLAGS=-Wall

all: main

main: main.c
	$(CC) $(CFLAGS) -o main main.c -lm -g -fopenmp

run: main
	mpirun -np 4 ./main $(N) $(S) $(T)

clean:
	rm -f main
