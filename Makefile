CC=mpicc
CFLAGS=-Wall

all: main

main: main.c
	$(CC) $(CFLAGS) -o main main.c

run: main
	mpirun -np $(T) ./main $(N) $(S)

clean:
	rm -f main
