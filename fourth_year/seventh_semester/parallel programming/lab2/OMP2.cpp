#include <omp.h>
#include <iostream>
#include <iomanip>

const int NUM_THREADS[3] = { 3, 6, 12 };
const int NUM_VECS = 4;
const int SIZE = 6000000;
const int Q_PARAM = 25;
const int RUNS = 20;
const double SUM = 4.0 * SIZE * Q_PARAM;

int main(int argc, char* argv[]) {

    std::cout << std::fixed << std::setprecision(5);

    for (int i = 0; i < 3; ++i) {

        omp_set_num_threads(NUM_THREADS[i]);

        float* a = new float[SIZE];
        float* b = new float[SIZE];
        float* c = new float[SIZE];
        float* d = new float[SIZE];

        for (int j = 0; j < SIZE; ++j) {
            a[j] = 1.f;
            b[j] = 1.f;
            c[j] = 1.f; 
            d[j] = 1.f;
        }

        double t0 = 0.0, t_s = 0.0, t_p = 0.0, t_r = 0.0, t_a = 0.0, t_c = 0.0;
        double sum_s = 0.0, sum_r = 0.0, sum_a = 0.0, sum_c = 0.0;

        for (int run = 0; run < RUNS; ++run) {

            t0 = omp_get_wtime();
            for (int j = 0; j < SIZE; ++j) {
                for (int q = 0; q < Q_PARAM; ++q) {
                    sum_s += a[j] + b[j] + c[j] + d[j];
                }
            }
            t_s += omp_get_wtime() - t0;

            t0 = omp_get_wtime();
            #pragma omp parallel shared(a, b, c, d)
            {
               
            }
            t_p += omp_get_wtime() - t0;

            t0 = omp_get_wtime();
            #pragma omp parallel for reduction(+:sum_r) shared(a, b, c, d)
            for (int j = 0; j < SIZE; ++j) {
                for (int q = 0; q < Q_PARAM; ++q) {
                    sum_r += a[j] + b[j] + c[j] + d[j];
                }
            }
            t_r += omp_get_wtime() - t0;

            t0 = omp_get_wtime();
            #pragma omp parallel for shared(a, b, c, d, sum_a)
            for (int j = 0; j < SIZE; ++j) {
                for (int q = 0; q < Q_PARAM; ++q) {
                    #pragma omp atomic
                    sum_a += a[j] + b[j] + c[j] + d[j];
                }
            }
            t_a += omp_get_wtime() - t0;

            t0 = omp_get_wtime();
            #pragma omp parallel for shared(a, b, c, d, sum_c) 
            for (int j = 0; j < SIZE; ++j) {
                for (int q = 0; q < Q_PARAM; ++q) {
                    #pragma omp critical
                    sum_c += a[j] + b[j] + c[j] + d[j];
                }
            }
            t_c += omp_get_wtime() - t0;
        }

        t_s /= RUNS;
        t_p /= RUNS;
        t_c /= RUNS;
        t_a /= RUNS;
        t_r /= RUNS;

        sum_s /= RUNS;
        sum_c /= RUNS;
        sum_a /= RUNS;
        sum_r /= RUNS;

        double a_cp = t_s / (t_c + t_p);
        double a_ap = t_s / (t_a + t_p);
        double a_rp = t_s / (t_r + t_p);
        double a_c = t_s / t_c;
        double a_a = t_s / t_a;
        double a_r = t_s / t_r;

        std::cout << "OpenMP Parameters:\n";
        std::cout << "  NUM_THREADS       = {3, 6, 12}\n";
        std::cout << "  Threads           = " << NUM_THREADS[i] << '\n';
        std::cout << "  Type              = float\n";
        std::cout << "  NUM_VECS          = " << NUM_VECS << '\n';
        std::cout << "  SIZE              = " << SIZE << '\n';
        std::cout << "  Q_PARAM           = " << Q_PARAM << '\n';
        std::cout << "  RUNS              = " << RUNS << '\n';
        std::cout << "  SUM               = " << SUM << '\n';

        std::cout << "\nTime (sec) (" << RUNS << " runs):\n";
        std::cout << "  ts  (sequential)  = " << t_s << '\n';
        std::cout << "  tp  (OpenMP init) = " << t_p << '\n';
        std::cout << "  tc  (critical)    = " << t_c << '\n';
        std::cout << "  ta  (atomic)      = " << t_a << '\n';
        std::cout << "  tr  (reduction)   = " << t_r << '\n';

        std::cout << "\nAcceleration (with init):\n";
        std::cout << "  Critical          = " << a_cp << '\n';
        std::cout << "  Atomic            = " << a_ap << '\n';
        std::cout << "  Reduction         = " << a_rp << '\n';

        std::cout << "\nAcceleration (without init):\n";
        std::cout << "  Critical          = " << a_c << '\n';
        std::cout << "  Atomic            = " << a_a << '\n';
        std::cout << "  Reduction         = " << a_r << '\n';

        std::cout << "\nChecksums:\n";
        std::cout << "  Expected          = " << SUM << '\n';
        std::cout << "  Sequential        = " << sum_s << '\n';
        std::cout << "  Critical          = " << sum_c << '\n';
        std::cout << "  Atomic            = " << sum_a << '\n';
        std::cout << "  Reduction         = " << sum_r << '\n';

        delete[] a;
        delete[] b;
        delete[] c;
        delete[] d;
    }

    return 0;
}
