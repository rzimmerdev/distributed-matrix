#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int testAndGetParams(int argc, char **argv, int rank, int *n, int *seed, int *t_threads);
int *createSendCounts(int n, int n_procs);
int *createDisplacements(int n, int n_procs, int *sendcounts);
int *generateCoordinateVector(int n, int n_procs, int *sendcounts);
int *receiveCoordSlice(int n, int n_procs, int my_rank, int *sendcounts);

int manhattanDistance(int x1, int y1, int z1, int x2, int y2, int z2);
double euclideanDistance(int x1, int y1, int z1, int x2, int y2, int z2);

int main(int argc, char **argv) {   
    int my_rank, n_procs, n, seed, t_threads;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

    if (testAndGetParams(argc, argv, my_rank, &n, &seed, &t_threads)) {
        MPI_Finalize();
        return 1;
    }
    srand(seed);

    int *sendcounts = createSendCounts(n, n_procs);
    
    int *X = receiveCoordSlice(n, n_procs, my_rank, sendcounts);
    int *Y = receiveCoordSlice(n, n_procs, my_rank, sendcounts);
    int *Z = receiveCoordSlice(n, n_procs, my_rank, sendcounts);

    // USAR OPENMP PARA PARALELIZAR O CALCULO DAS QUANTIDADES LOCAIS
    //  POSSIVELMENTE CRIAR VETOR DE MÁXIMOS E MÍNIMOS EM RELAÇÃO A CADA PONTO.

    // USAR DEPOIS MPI PARA REDUZIR AS QUANTIDADES ENTRE OS PROCESSOS
    //  UM DESAFIO DESTA ETAPA É PERCEBER O PADRÃO DE QUAIS PONTOS TÊM DE SER
    //  COMPARADOS COM QUAIS, UMA VEZ QUE OS PONTOS ESTÃO INTERCALADOS. 

    free(X);
    free(Y);
    free(Z);
    free(sendcounts);

    MPI_Finalize();

    return 0;
}

int testAndGetParams(int argc, char **argv, int rank, int *n, int *seed, int *t_threads) {
    if (argc != 3+1) {
        if (rank == 0) {
            fprintf(stderr, "Usage: %s <N> <Seed> <T_Threads>\n", argv[0]);
        }
        return 1;
    }   
    
    *n = atoi(argv[1]);
    *n *= *n; // Linearized Matrix
    *seed = atoi(argv[2]);
    *t_threads = atoi(argv[3]);

    return 0; 
}

int *createSendCounts(int n, int n_procs) {
    int *sendcounts = (int *)malloc(n_procs * sizeof(int));
    int quotient = n / n_procs;
    int remainder = n % n_procs;

    for (int i = 0; i < n_procs; i++) {
        sendcounts[i] = quotient;
    }
    
    for (int i = 0; i < remainder; i++) {
        sendcounts[i]++;
    }

    return sendcounts;
}

int *createDisplacements(int n, int n_procs, int *sendcounts) {
    int *displs = (int *)malloc(n_procs * sizeof(int));

    displs[0] = 0;
    for (int i = 1; i < n_procs; i++) {
        displs[i] = displs[i - 1] + sendcounts[i - 1];
    }

    return displs;
}

int *generateCoordinateVector(int n, int n_procs, int *sendcounts) {
    int *coord_vect = (int *)malloc(n * sizeof(int));
    int quotient = n / n_procs;
    int remainder = n % n_procs;

    for (int i = 0; i < quotient; i++) {
        for (int j = i, proc = 0; proc < n_procs; j += sendcounts[proc++]) {
            coord_vect[j] = rand() % 100;
        }
    }

    for (int i = 0; i < remainder; i++) {
        int index = quotient + (i * (quotient + 1));
        coord_vect[index] = rand() % 100;
    }

    return coord_vect;
}

int *receiveCoordSlice(int n, int n_procs, int my_rank, int *sendcounts) {
    int *X = NULL;
    int *displs = createDisplacements(n, n_procs, sendcounts);
    int *local_X = (int *)malloc(sendcounts[my_rank] * sizeof(int));
    if (my_rank == 0) {
        X = generateCoordinateVector(n, n_procs, sendcounts);
    } 

    MPI_Scatterv(X, sendcounts, displs, MPI_INT, local_X, sendcounts[my_rank], MPI_INT, 0, MPI_COMM_WORLD);

    free(displs);
    if (X != NULL) {
        free(X);
    }

    return local_X;
}

int manhattanDistance(int x1, int y1, int z1, int x2, int y2, int z2) {
    return abs(x1 - x2) + abs(y1 - y2) + abs(z1 - z2);
}

double euclideanDistance(int x1, int y1, int z1, int x2, int y2, int z2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));
}
