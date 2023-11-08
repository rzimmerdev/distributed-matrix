#include <stdio.h>
#include <mpi.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    int rank, size;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the rank and size from the MPI environment
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Print off a hello world message from the MPI process
    printf("Hello from MPI process %d out of %d\n", rank, size);

    // Begin of the OpenMP parallel region
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        printf("Hello from thread %d of MPI process %d\n", thread_id, rank);
    } // End of the OpenMP parallel region

    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}
