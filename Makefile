CC=mpicc
CFLAGS=-Wall

all: main

main: main.c
	$(CC) $(CFLAGS) -o main main.c

run: main
	mpirun -np 4 ./main $(N) $(S) $(T)

clean:
	rm -f main
