#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define LCG_A 1103515245
#define LCG_C 12345
#define LCG_M 1 << 31

unsigned int next_rand(unsigned int seed) {
    return (LCG_A * seed + LCG_C) % (LCG_M) % 100;
}

void generate_matrix_portion(int *matrix_portion, int portion_size, int total_size, int rank, unsigned int seed) {
    unsigned long long jump_ahead = rank * portion_size * total_size;
    for (unsigned long long i = 0; i < jump_ahead; i++) {
        seed = next_rand(seed);
    }
    for (int i = 0; i < portion_size * total_size; i++) {
        seed = next_rand(seed);
        matrix_portion[i] = seed;
    }
}

int main(int argc, char *argv[]) {
    int N, S;
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == 0) {
            fprintf(stderr, "Usage: %s N S\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    N = atoi(argv[1]);
    S = atoi(argv[2]);

    int portion_size = (N + size - 1) / size;
    int *matrix_portion = (int *)malloc(portion_size * N * sizeof(int));
    if (matrix_portion == NULL) {
        fprintf(stderr, "Cannot allocate memory for the matrix portion.\n");
        MPI_Finalize();
        return 1;
    }

    generate_matrix_portion(matrix_portion, portion_size, N, rank, S);

    // Print matrix sequentially
    // If root process, print self portion and then receive and print other portions
    // If not root process, send self portion to root process
    if (rank != 0) {
        MPI_Send(matrix_portion, portion_size * N, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        for (int i = 0; i < portion_size * N; i++) {
            printf("%d ", matrix_portion[i]);
            if ((i + 1) % N == 0) {
                printf("\n");
            }
        }
        for (int i = 1; i < size; i++) {
            MPI_Recv(matrix_portion, portion_size * N, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0; j < portion_size * N; j++) {
                printf("%d ", matrix_portion[j]);
                if ((j + 1) % N == 0) {
                    printf("\n");
                }
            }
        }
    }

    free(matrix_portion);
    MPI_Finalize();
    return 0;
}
