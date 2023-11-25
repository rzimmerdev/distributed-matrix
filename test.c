#include <mpi.h>
#include <omp.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int provided, rank, size, n = 1000;
    double a[n], b[n], result = 0.0;

    // Initialize the MPI environment
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    if (provided < MPI_THREAD_FUNNELED) {
        printf("MPI does not provide the needed threading level\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Initialize arrays
#pragma omp parallel for simd
    for (int i = 0; i < n; i++) {
        a[i] = i * 0.5;
        b[i] = i * 2.0;
    }

    // Perform computation using taskloop and simd
#pragma omp parallel
#pragma omp single
#pragma omp taskloop simd reduction(+:result)
    for (int i = 0; i < n; i++) {
        result += a[i] * b[i];
    }

    // Use MPI to reduce the results across all processes
    double global_result;
    MPI_Reduce(&result, &global_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Total sum = %f\n", global_result);
    }

    // Finalize the MPI environment
    MPI_Finalize();

    return 0;
}
