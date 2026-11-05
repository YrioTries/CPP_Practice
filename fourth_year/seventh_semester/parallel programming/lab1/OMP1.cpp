#include <math.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>
#include <locale.h>
#include <stdio.h>


int main(int argc, char* argv[])
{
    omp_set_num_threads(8);

    double total_time = 0;
    int i = 0;

    for (i = 0; i < 12; i++) {
        double start = omp_get_wtime();

        int nTheads, theadNum;
#pragma omp parallel  private(nTheads, theadNum) //параллел обл
        {
            nTheads = omp_get_num_threads(); //колво потоков
            theadNum = omp_get_thread_num(); //номер текущего потока
            printf("OpenMP thread %d from %d threads \n", theadNum, nTheads);
        }
        double end = omp_get_wtime();
        total_time += (end - start);
    }

    printf("Average time over 12 runs: %f seconds\n", total_time / 12);

    return 0;
}
