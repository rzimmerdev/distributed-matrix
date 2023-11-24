CC=mpicc
CFLAGS=-Wall -fopenmp
HOSTFILE=hostfile.txt

P=4
NUM_RUNS=5

all: main

main: main.c
	$(CC) $(CFLAGS) -o main main.c -lm -g

run: main
	mpirun -np $(P) --hostfile $(HOSTFILE) ./main $(N) $(S) $(T)

time: main
	@echo "Calculating average time over $(NUM_RUNS) runs..."
	@{ \
	total_millis=0; \
	for i in $$(seq 1 $(NUM_RUNS)); do \
		duration=$$(/usr/bin/time -f "%e" mpirun -np $(P) --hostfile=$(HOSTFILE) ./main $(N) $(S) $(T) 2>&1 | tr -d '\n'); \
		total_millis=$$(echo "$$total_millis + $$duration" | bc); \
	done; \
	avg_millis=$$(echo "scale=3; $$total_millis / $(NUM_RUNS)" | bc); \
	echo "Average time: $$avg_millis seconds"; \
	}

clean:
	rm -f main
