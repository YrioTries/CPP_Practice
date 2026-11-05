#include "mpi.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char* argv[]) {
    int rank, size;
    double start_time, end_time, total_time = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int j;

    // Цикл для усреднения времени
    for (j = 0; j < 12; ++j) {
        // Синхронизация всех процессов перед началом измерения времени
        MPI_Barrier(MPI_COMM_WORLD);

        // Начало измерения времени
        start_time = MPI_Wtime();
        printf("Hello world from process %d from total number of %d\n", rank, size);
        // Окончание измерения времени
        end_time = MPI_Wtime();
        double elapsed_time = end_time - start_time;

        total_time += elapsed_time;

        // Синхронизация всех процессов после завершения работы
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Усредняем время выполнения 
    double global_total_time;
    double avg_time;
    MPI_Reduce(&total_time, &global_total_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        avg_time = global_total_time / 12;
        printf("Average execution time over %d runs on %d processes: %f seconds\n", 12, size, avg_time);
    }
    MPI_Finalize();
    return 0;
}
