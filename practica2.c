#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <mpi.h>

int MPI_FlattreeColectiva(void *buff, void *recvbuff, int count,
                            MPI_Datatype datatype, int root, MPI_Comm comm) {
    int rank, n_procs;
    double n, total;
    MPI_Status status;

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &n_procs);

    if (rank == root) {
        total = *(double *)buff;
        for (int i = 0; i < n_procs; i++) {
            if (i == root) continue;
            MPI_Recv(&n, count, datatype, MPI_ANY_SOURCE, 0, comm, &status);
            total += n;
        }
        *(double *)recvbuff = total;
    } else {
        for (int i = 0; i < n_procs; i++) {
            if (i == root) continue;
            MPI_Send(buff, 1, datatype, 0, 0,  comm);
        }
    }
    return MPI_SUCCESS;
}

int MPI_BinomialBcast(void *buf, int count, MPI_Datatype datatype,
                int root, MPI_Comm comm) {
    int i = 1, mask = 0x1;
    int rank, n_procs;

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &n_procs);

    while (mask < n_procs) {

        if (rank < mask && (rank + mask < n_procs)) {
            MPI_Send(buf, count, datatype, (rank + mask), 0, comm);
        } else if (rank >= mask && rank < mask << 1) {
            MPI_Recv(buf, count, datatype, (rank - mask), 0, comm, MPI_STATUS_IGNORE);
        }

        mask <<= 1;
    }
    return MPI_SUCCESS;
}


int main(int argc, char *argv[]) {
    int i, done = 0, n, count;
    double PI25DT = 3.141592653589793238462643;
    double pi, x, y, z;

    // variables for MPI
    int n_procs, rank;
    double total_pi;
    MPI_Status status;

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

        MPI_BinomialBcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

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

        if (MPI_FlattreeColectiva(&pi, &total_pi, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD) != MPI_SUCCESS) {
            printf("Error\n");
        } else if (rank == 0) {
            printf("pi is approx. %.16f, Error is %.16f\n", total_pi, fabs(total_pi - PI25DT));
        }
    }
    MPI_Finalize();
}