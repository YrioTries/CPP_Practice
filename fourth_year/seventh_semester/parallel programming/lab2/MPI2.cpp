#include <mpi.h>
#include <iostream>
#include <iomanip>

const int NUM_VECS = 4;
const int SIZE = 6000000;
const int Q_PARAM = 25;
const int RUNS = 20;
const double SUM = 4.0 * SIZE * Q_PARAM;

int main(int argc, char* argv[]) {

    MPI_Init(&argc, &argv);

    int worldSize, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::cout << std::fixed << std::setprecision(5);

    float* a = new float[SIZE];
    float* b = new float[SIZE];
    float* c = new float[SIZE];
    float* d = new float[SIZE];

    if (rank == 0) {
        for (int i = 0; i < SIZE; ++i) {
            a[i] = 1.f;
            b[i] = 1.f;
            c[i] = 1.f;
            d[i] = 1.f;
        }
    }

    double t_s = 0.0, t_b = 0.0, t_r = 0.0, t_pp = 0.0;
    double sum_s = 0.0, sum_r = 0.0, sum_pp = 0.0;

    for (int run = 0; run < RUNS; ++run) {

        double t0 = MPI_Wtime();
        MPI_Bcast(a, SIZE, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Bcast(b, SIZE, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Bcast(c, SIZE, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Bcast(d, SIZE, MPI_FLOAT, 0, MPI_COMM_WORLD);
        t_b += MPI_Wtime() - t0;

        if (rank == 0) {
            t0 = MPI_Wtime();
            for (int i = 0; i < SIZE; ++i) {
                for (int q = 0; q < Q_PARAM; ++q) {
                    sum_s += a[i] + b[i] + c[i] + d[i];
                }
            }
            t_s += MPI_Wtime() - t0;
        }

        int chunk = SIZE / worldSize;
        int start = rank * chunk;
        int end = (rank == worldSize - 1) ? SIZE : start + chunk;

        MPI_Barrier(MPI_COMM_WORLD);
        t0 = MPI_Wtime();
        double sum_loc = 0.0;
        for (int i = start; i < end; ++i) {
            for (int q = 0; q < Q_PARAM; ++q) {
                sum_loc += a[i] + b[i] + c[i] + d[i];
            }
        }
        // замер с барьером
        if (rank == 0) {
            sum_pp += sum_loc;
            for (int i = 1; i < worldSize; ++i) {
                MPI_Recv(&sum_loc, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                sum_pp += sum_loc;
            }
        }
        else {
            MPI_Send(&sum_loc, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        t_pp += MPI_Wtime() - t0;

        MPI_Barrier(MPI_COMM_WORLD);
        t0 = MPI_Wtime();
        sum_loc = 0.0;
        for (int i = start; i < end; ++i) {
            for (int q = 0; q < Q_PARAM; ++q) {
                sum_loc += a[i] + b[i] + c[i] + d[i];
            }
        }
        double sum_temp = 0.0;
        MPI_Reduce(&sum_loc, &sum_temp, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            sum_r += sum_temp;
        }
        MPI_Barrier(MPI_COMM_WORLD);
        t_r += MPI_Wtime() - t0;
    }

    if (rank == 0) {

        t_s /= RUNS;
        t_b /= RUNS;
        t_pp /= RUNS;
        t_r /= RUNS;

        sum_s /= RUNS;
        sum_pp /= RUNS;
        sum_r /= RUNS;

        double a_ppb = t_s / (t_pp + t_b);
        double a_rb = t_s / (t_r + t_b);
        double a_pp = t_s / t_pp;
        double a_r = t_s / t_r;

        std::cout << "MPI Parameters:\n";
        std::cout << "  Processes            = " << worldSize << '\n';
        std::cout << "  Type                 = float\n";
        std::cout << "  NUM_VECS             = " << NUM_VECS << '\n';
        std::cout << "  SIZE                 = " << SIZE << '\n';
        std::cout << "  Q_PARAM              = " << Q_PARAM << '\n';
        std::cout << "  RUNS                 = " << RUNS << '\n';
        std::cout << "  Expected             = " << SUM << '\n';

        std::cout << "\nTime (sec) (" << RUNS << " runs):\n";
        std::cout << "  ts (sequential)      = " << t_s << '\n';
        std::cout << "  tb (broadcast)       = " << t_b << '\n';
        std::cout << "  tpp (point-to-point) = " << t_pp << '\n';
        std::cout << "  tr (reduce)          = " << t_r << '\n';

        std::cout << "\nAccelerations (with broadcast):\n";
        std::cout << "  Point-to-Point       = " << a_ppb << '\n';
        std::cout << "  Reduce               = " << a_rb << '\n';

        std::cout << "\nAccelerations (without broadcast):\n";
        std::cout << "  Point-to-Point       = " << a_pp << '\n';
        std::cout << "  Reduce               = " << a_r << '\n';

        std::cout << "\nChecksums:\n";
        std::cout << "  Expected             = " << SUM << '\n';
        std::cout << "  Sequential           = " << sum_s << '\n';
        std::cout << "  Point-to-Point       = " << sum_pp << '\n';
        std::cout << "  Reduce               = " << sum_r << '\n';
    }

    delete[] a;
    delete[] b;
    delete[] c;
    delete[] d;

    MPI_Finalize();
    return 0;
}
