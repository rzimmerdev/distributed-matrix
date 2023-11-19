#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define DATA_TAG 0
#define MAX_TAG 1
#define MIN_TAG 2
#define INTERVAL 100
#define INT_MAX 2147483647

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((b) > (a) ? (a) : (b))
#define ABS(a) ((a) > 0 ? (a) : -(a))


void generate_blocks(int *block, int block_size, unsigned int seed) {
    srand(seed);
    for (int i = 0; i < block_size; ++i) {
        block[i] = rand() % INTERVAL;
    }
}

int main(int argc, char **argv) {
    int rank, n, s, t;
    int i, j;

    int *block;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &t);

    if (argc != 3) {
        if (rank == 0) {
            fprintf(stderr, "Usage: %s N S\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    n = atoi(argv[1]);
    n *= n;
    s = atoi(argv[2]);

    // Save a block of size n / t for each process
    int block_size = n / t + (rank == 0);
    int default_size = n / t;

    block = (int *) malloc(block_size * sizeof(int));
    // If master, generate and send blocks to each process (including self)
    // Send non-blocking to avoid deadlock
    // Receive blocking to avoid data loss
    if (rank == 0) {
        for (int i = 1; i < t; ++i) {
            generate_blocks(block, default_size, s + i);
            MPI_Send(block, default_size, MPI_INT, i, DATA_TAG, MPI_COMM_WORLD);
        }
        generate_blocks(block, block_size, s);

    } else {
        MPI_Recv(block, default_size, MPI_INT, 0, DATA_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // For each point in block, calculate its Euclidean distance to all points with higher index in vector
    // For this, set j = i + 1 and calculate distance between block[i] and block[j]
    // Then, for p = current rank, for rank = p + 1 to t, calculate distance between block[i] and block[j] for each points
    // Return maximum distance and minimum for current block
    int p, max = 0, min = INT_MAX;

    // Isend to all processes with lower rank
    MPI_Request *requests;
    if (rank > 0) {
        requests = (MPI_Request *) malloc(rank * sizeof(MPI_Request));
        for (p = 0; p < rank; p++) {
            MPI_Isend(block, block_size, MPI_INT, p, DATA_TAG, MPI_COMM_WORLD, &requests[p]);
        }
    }

    for (i = 0; i < block_size; ++i) {
        for (j = i + 1; j < block_size; ++j) {
            int distance = abs(block[i] - block[j]);
            max = MAX(max, distance);
            min = MIN(min, distance);
        }
    }

    int *recv_block = (int *) malloc(default_size * sizeof(int));
    int p_max, p_min;

    for (int p = rank + 1; p < t; p++) {
        p_max = 0, p_min = INT_MAX;
        MPI_Recv(recv_block, default_size, MPI_INT, p, DATA_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (i = 0; i < default_size; i++) {
            for (j = 0; j < block_size; j++) {
                int distance = abs(recv_block[i] - block[j]);
                p_max = MAX(p_max, distance);
                p_min = MIN(p_min, distance);
            }
        }

        max = MAX(max, p_max);
        min = MIN(min, p_min);
    }

    if (rank != 0) {
        printf("Process %d\n", rank);
        MPI_Waitall(rank, requests, MPI_STATUSES_IGNORE);
        MPI_Send(&max, 1, MPI_INT, 0, MAX_TAG, MPI_COMM_WORLD);
        MPI_Send(&min, 1, MPI_INT, 0, MIN_TAG, MPI_COMM_WORLD);
    }
    else {
        int global_max = max, global_min = min;

        for (int p = 1; p < t; p++) {
            MPI_Recv(&max, 1, MPI_INT, p, MAX_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&min, 1, MPI_INT, p, MIN_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            global_max = MAX(global_max, max);
            global_min = MIN(global_min, min);
        }
        printf("Max: %d; Min: %d\n", global_max, global_min);
    }
    free(block);
    MPI_Finalize();
    return 0;
}
