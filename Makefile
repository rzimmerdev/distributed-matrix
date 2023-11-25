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

# Run the program NUM_RUNS times and calculate the average
# Must be extremely precise with time measurements
# elements is = N^2
time: main
	@echo "Number of runs: $(NUM_RUNS)"
	@echo "Number of elements: $$(echo "$N * $N" | bc)"
	@echo "Timing runs..."
	@TOTAL_TIME="0"; \
	for run in `seq 1 $(NUM_RUNS)`; do \
		START=$$(date +%s.%N); \
		mpirun -np $(P) --hostfile $(HOSTFILE) ./main $(N) $(S) $(T) > /dev/null; \
		END=$$(date +%s.%N); \
		RUN_TIME=$$(echo "scale=9; $$END - $$START" | bc); \
		echo "Run $$run time: $$RUN_TIME seconds"; \
		TOTAL_TIME=$$(echo "scale=9; $$TOTAL_TIME + $$RUN_TIME" | bc); \
	done; \
	AVG_TIME=$$(echo "scale=9; $$TOTAL_TIME / $(NUM_RUNS)" | bc); \
	echo "Average time: $$AVG_TIME s"


test: version.c
	$(CC) $(CFLAGS) -o version version.c -lm -g
	mpirun -np $(P) --hostfile localhost.txt ./version

clean:
	rm -f main
