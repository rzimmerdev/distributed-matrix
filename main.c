#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define X_TAG 0
#define Y_TAG 1
#define Z_TAG 2

#define INF 1e9

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((b) > (a) ? (a) : (b))

int get_params(int argc, char **argv, int *n, int *s, int *t);
int *get_sendcounts(int n, int n_procs);
int *receive_displacement(int n_procs, int *sendcounts);
int *create_vector(int n, int n_procs, int *sendcounts);
int *receive_coord(int n, int n_procs, int rank, int *sendcounts);
int manhattan(int x1, int y1, int z1, int x2, int y2, int z2);
double euclidean(int x1, int y1, int z1, int x2, int y2, int z2);


int main(int argc, char **argv) {
    int n, s, t;

    if (get_params(argc, argv, &n, &s, &t)) {
        return 1;
    }

    // From here on, n is the vector size, not the original one
    int rank, n_procs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

    srand(s);

    int *sendcounts = get_sendcounts(n, n_procs);

    int *X = receive_coord(n, n_procs, rank, sendcounts);
    int *Y = receive_coord(n, n_procs, rank, sendcounts);
    int *Z = receive_coord(n, n_procs, rank, sendcounts);

    int *X_p = malloc(sendcounts[0] * sizeof(int));
    int *Y_p = malloc(sendcounts[0] * sizeof(int));
    int *Z_p = malloc(sendcounts[0] * sizeof(int));

    double *max_e_local = calloc(sendcounts[rank], sizeof(double));
    double *min_e_local = malloc(sendcounts[rank] * sizeof(double));

    int *max_m_local = calloc(sendcounts[rank], sizeof(int));
    int *min_m_local = malloc(sendcounts[rank] * sizeof(int));

    for (int i = 0; i < sendcounts[rank]; i++) {
        min_e_local[i] = (double) INF;
        min_m_local[i] = (int) INF;
    }

    int max_m = 0, min_m = (int) INF;
    double max_e = 0, min_e = (double) INF;

    int p, i, j, offset_start = 0;

    MPI_Request *requests = (MPI_Request *) malloc((3 * n_procs) * sizeof(MPI_Request));

    for (p = 0; p < n_procs; p++) {
        MPI_Isend(X, sendcounts[rank], MPI_INT, p, X_TAG, MPI_COMM_WORLD, &requests[3 * p + 0]);
        MPI_Isend(Y, sendcounts[rank], MPI_INT, p, Y_TAG, MPI_COMM_WORLD, &requests[3 * p + 1]);
        MPI_Isend(Z, sendcounts[rank], MPI_INT, p, Z_TAG, MPI_COMM_WORLD, &requests[3 * p + 2]);
    }

    // Total memory used = 6 * n / n_procs + 4 * n / n_procs
    // = 10 * n / n_procs
    // So if n_procs = 10, each process will use n memory
    for (p = 0; p < n_procs; p++) {
        MPI_Recv(X_p, sendcounts[p], MPI_INT, p, X_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(Y_p, sendcounts[p], MPI_INT, p, Y_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(Z_p, sendcounts[p], MPI_INT, p, Z_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (i = 0; i < sendcounts[rank]; i++) {
            offset_start = p > rank ? i : i + 1;
            for (j = offset_start; j < sendcounts[p]; j++) {
                double e = euclidean(X[i], Y[i], Z[i], X_p[j], Y_p[j], Z_p[j]);
                int m = manhattan(X[i], Y[i], Z[i], X_p[j], Y_p[j], Z_p[j]);

                printf("Distnace between (%d, %d, %d) and (%d, %d, %d) is %d\n", X[i], Y[i], Z[i], X_p[j], Y_p[j], Z_p[j], m);

                max_e_local[i] = MAX(max_e_local[i], e);
                min_e_local[i] = MIN(min_e_local[i], e);

                max_m_local[i] = MAX(max_m_local[i], m);
                min_m_local[i] = MIN(min_m_local[i], m);

                max_e = MAX(max_e, e);
                min_e = MIN(min_e, e);

                max_m = MAX(max_m, m);
                min_m = MIN(min_m, m);
            }
        }
    }

    double max_sum_e = 0, min_sum_e = 0;
    int max_sum_m = 0, min_sum_m = 0;

    for (i = 0; i < sendcounts[rank]; i++) {
        max_sum_e += max_e_local[i];
        min_sum_e += min_e_local[i] < INF ? min_e_local[i] : 0;

        max_sum_m += max_m_local[i];
        min_sum_m += min_m_local[i] < INF ? min_m_local[i] : 0;

//        printf("Process %d, point %d, min_sum_m: %d\n", rank, i, min_sum_m);
//        printf("Process %d, point %d, max_sum_m: %d\n", rank, i, max_sum_m);
    }

    int max_m_global, min_m_global;
    int max_sum_m_global, min_sum_m_global;

    double max_e_global, min_e_global;
    double max_sum_e_global, min_sum_e_global;

    MPI_Reduce(&max_sum_e, &max_sum_e_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&min_sum_e, &min_sum_e_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&max_e, &max_e_global, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&min_e, &min_e_global, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);

    MPI_Reduce(&max_sum_m, &max_sum_m_global, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&min_sum_m, &min_sum_m_global, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&max_m, &max_m_global, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&min_m, &min_m_global, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Distância Manhattan mínima: %d (soma min: %d) ", min_m_global, min_sum_m_global);
        printf("e máxima: %d (soma max: %d)\n", max_m_global, max_sum_m_global);

        printf("Distância Euclidiana mínima: %.2lf (soma min: %.2lf) ", min_e_global, min_sum_e_global);
        printf("e máxima: %.2lf (soma max: %.2lf)\n", max_e_global, max_sum_e_global);
    }

    for (p = 0; p < n_procs; p++) {
        MPI_Wait(&requests[3 * p + 0], MPI_STATUS_IGNORE);
        MPI_Wait(&requests[3 * p + 1], MPI_STATUS_IGNORE);
        MPI_Wait(&requests[3 * p + 2], MPI_STATUS_IGNORE);
    }

    free(requests);
    free(max_e_local);
    free(min_e_local);
    free(max_m_local);
    free(min_m_local);

    free(X_p);
    free(Y_p);
    free(Z_p);

    free(X);
    free(Y);
    free(Z);
    free(sendcounts);

    MPI_Finalize();

    return 0;
}

int get_params(int argc, char **argv, int *n, int *s, int *t) {
    if (argc != 3 + 1) {
        fprintf(stderr, "Usage: %s <N> <Seed> <T_Threads>\n", argv[0]);
        return 1;
    }
    *n = atoi(argv[1]);
    *n *= *n;
    *s = atoi(argv[2]);
    *t = atoi(argv[3]);
    return 0;
}

int *get_sendcounts(int n, int n_procs) {
    int *sendcounts = (int *) malloc(n_procs * sizeof(int));
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

int *receive_displacement(int n_procs, int *sendcounts) {
    int *displacement = (int *) malloc(n_procs * sizeof(int));

    displacement[0] = 0;
    for (int i = 1; i < n_procs; i++) {
        displacement[i] = displacement[i - 1] + sendcounts[i - 1];
    }

    return displacement;
}

int *create_vector(int n, int n_procs, int *sendcounts) {
    int *vector = (int *) malloc(n * sizeof(int));
    int quotient = n / n_procs;
    int remainder = n % n_procs;

    for (int i = 0; i < quotient; i++) {
        for (int j = i, proc = 0; proc < n_procs; j += sendcounts[proc++]) {
            vector[j] = rand() % 100;
        }
    }

    for (int i = 0; i < remainder; i++) {
        int index = quotient + (i * (quotient + 1));
        vector[index] = rand() % 100;
    }

    return vector;
}

int *receive_coord(int n, int n_procs, int rank, int *sendcounts) {
    int *coord = NULL;
    int *displacements = receive_displacement(n_procs, sendcounts);
    int *local_coord = (int *) malloc(sendcounts[rank] * sizeof(int));
    if (rank == 0) {
        coord = create_vector(n, n_procs, sendcounts);
    }

    MPI_Scatterv(coord, sendcounts, displacements, MPI_INT, local_coord, sendcounts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    free(displacements);
    if (coord != NULL) {
        free(coord);
    }

    return local_coord;
}

int manhattan(int x1, int y1, int z1, int x2, int y2, int z2) {
    return abs(x1 - x2) + abs(y1 - y2) + abs(z1 - z2);
}

double euclidean(int x1, int y1, int z1, int x2, int y2, int z2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));
}
