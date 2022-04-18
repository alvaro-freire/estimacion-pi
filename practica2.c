#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int i, done = 0, n, count;
    double PI25DT = 3.141592653589793238462643;
    double pi, x, y, z;

    // variables for MPI
    int n_proc, rank, j;
    double total_pi;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // rank 0 -> master
    while (!done) {
        if (rank == 0) {    // master sends data
            printf("Enter the number of points: (0 quits) \n");
            scanf("%d", &n);

            for (j = 1; j < n_proc; ++j) {
                MPI_Send(&n, 1, MPI_INT, j, 0, MPI_COMM_WORLD);
            }
            total_pi = 0;
        } else {    // worker receives data
            MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        if (n == 0) {
            break;
        }

        count = 0;
        for (i = 1; i <= n; i += n_proc) {
            x = ((double) rand()) / ((double) RAND_MAX);
            y = ((double) rand()) / ((double) RAND_MAX);
            z = sqrt((x * x) + (y * y));

            if (z <= 1.0) {
                count++;
            }
        }
        pi = ((double) count / (double) n) * 4.0;

        if (rank == 0) {    // master receives results
            total_pi = pi;

            for (j = 1; j < n_proc; j++) {
                MPI_Recv(&pi, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                total_pi += pi;
            }
            printf("pi is approx. %.16f, Error is %.16f\n", total_pi, fabs(total_pi - PI25DT));
        } else {    // worker sends results
            MPI_Send(&pi, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
}