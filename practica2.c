#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int i, done = 0, n, count;
    double PI25DT = 3.141592653589793238462643;
    double pi, x, y, z;

    // variables for MPI
    int n_procs, rank;
    double total_pi;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // rank 0 -> master
    while (!done) {
        if (rank == 0) {    // master sends data
            printf("Enter the number of points: (0 quits) \n");
            scanf("%d", &n);
            total_pi = 0;
        }

        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (n == 0) {
            break;
        }

        count = 0;
        for (i = 1; i <= n; i += n_procs) {
            x = ((double) rand()) / ((double) RAND_MAX);
            y = ((double) rand()) / ((double) RAND_MAX);
            z = sqrt((x * x) + (y * y));

            if (z <= 1.0) {
                count++;
            }
        }
        pi = ((double) count / (double) n) * 4.0;

        MPI_Reduce(&pi, &total_pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        if (rank == 0) {
            printf("pi is approx. %.16f, Error is %.16f\n", total_pi, fabs(total_pi - PI25DT));
        }
    }
    MPI_Finalize();
}