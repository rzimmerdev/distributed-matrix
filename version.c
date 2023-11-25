// Author: rzimmerdev & rinderomna
// Get the current installed version of OpenMP(I)

#include <stdio.h>
#include <omp.h>
#include <mpi.h>
#include <stdlib.h>

#

int main(int argc, char *argv[]) {
    int rank, size, provided;
    char name[MPI_MAX_PROCESSOR_NAME];
    int len;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(name, &len);

    if (provided < MPI_THREAD_FUNNELED) {
        printf("Error: the MPI library does not provide the required thread support\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    if (rank == 0) {
        // Get MPI version
        int version, subversion;
        MPI_Get_version(&version, &subversion);
        printf("MPI version: %d.%d\n", version, subversion);

        // Get OpenMP version
        #ifdef _OPENMP
        printf("OpenMP version (from macro _OPENMP): %d\n", _OPENMP);
        #else
        printf("OpenMP is not supported.\n");
        #endif
    }


    MPI_Finalize();
    return 0;
}