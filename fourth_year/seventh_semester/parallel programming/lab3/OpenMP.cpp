#include <omp.h>
#include <iostream>
#include <iomanip>

const int NUM_THREADS[3] = { 3, 6, 12 };
const int NUM_VECS = 4;
const int SIZE = 6000000;
const int Q_PARAM = 25;
const int RUNS = 20;

int main(int argc, char* argv[]) {

    std::cout << std::fixed << std::setprecision(5);

    for (int i = 0; i < 3; ++i) {

        omp_set_num_threads(NUM_THREADS[i]);

        // Выделение памяти под 4 входных вектора (a, b, c, d) и результирующий (e)
        float* a = new float[SIZE];
        float* b = new float[SIZE];
        float* c = new float[SIZE];
        float* d = new float[SIZE];
        float* e = new float[SIZE];

        for (int j = 0; j < SIZE; ++j) {
            a[j] = 1.f;
            b[j] = 1.f;
            c[j] = 1.f;
            d[j] = 1.f;
            e[j] = 0.f;
        }

        double t0 = 0.0, t_s_mul = 0.0, t_p = 0.0, t_st = 0.0, t_d = 0.0, t_g = 0.0;

        for (int run = 0; run < RUNS; ++run) {

            // 1. Последовательное выполнение
            t0 = omp_get_wtime();
            for (int j = 0; j < SIZE; ++j) {
                for (int q = 0; q < Q_PARAM; ++q) {
                    e[j] = a[j] + b[j] + c[j] + d[j];
                }
            }
            t_s_mul += omp_get_wtime() - t0;

            // 2. Замер накладных расходов на создание/синхронизацию потоков
            t0 = omp_get_wtime();
#pragma omp parallel
            {
                // empty parallel structured block
            }
            t_p += omp_get_wtime() - t0;

            // 3. Параллельное выполнение: schedule(static)
            t0 = omp_get_wtime();
#pragma omp parallel for schedule(static)
            for (int j = 0; j < SIZE; ++j) {
                for (int q = 0; q < Q_PARAM; ++q) {
                    e[j] = a[j] + b[j] + c[j] + d[j];
                }
            }
            t_st += omp_get_wtime() - t0;

            // 4. Параллельное выполнение: schedule(dynamic)
            t0 = omp_get_wtime();
#pragma omp parallel for schedule(dynamic)
            for (int j = 0; j < SIZE; ++j) {
                for (int q = 0; q < Q_PARAM; ++q) {
                    e[j] = a[j] + b[j] + c[j] + d[j];
                }
            }
            t_d += omp_get_wtime() - t0;

            // 5. Параллельное выполнение: schedule(guided)
            t0 = omp_get_wtime();
#pragma omp parallel for schedule(guided)
            for (int j = 0; j < SIZE; ++j) {
                for (int q = 0; q < Q_PARAM; ++q) {
                    e[j] = a[j] + b[j] + c[j] + d[j];
                }
            }
            t_g += omp_get_wtime() - t0;
        }

        t_s_mul /= RUNS;
        t_p /= RUNS;
        t_st /= RUNS;
        t_d /= RUNS;
        t_g /= RUNS;

        double a_stp = t_s_mul / (t_st + t_p);
        double a_dp = t_s_mul / (t_d + t_p);
        double a_gp = t_s_mul / (t_g + t_p);
        double a_st = t_s_mul / t_st;
        double a_d = t_s_mul / t_d;
        double a_g = t_s_mul / t_g;

        std::cout << "\nOpenMP Parameters:\n";
        std::cout << "  NUM_THREADS         = {3, 6, 12}\n";
        std::cout << "  Threads             = " << NUM_THREADS[i] << '\n';
        std::cout << "  Type                = float\n";
        std::cout << "  NUM_VECS            = " << NUM_VECS << '\n';
        std::cout << "  SIZE                = " << SIZE << '\n';
        std::cout << "  Q_PARAM             = " << Q_PARAM << '\n';
        std::cout << "  RUNS                = " << RUNS << '\n';

        std::cout << "\nTime (sec) (" << RUNS << " runs):\n";
        std::cout << "  tsmul (sequential)  = " << t_s_mul << '\n';
        std::cout << "  tp (OpenMP init)    = " << t_p << '\n';
        std::cout << "  tst (static)        = " << t_st << '\n';
        std::cout << "  td (dynamic)        = " << t_d << '\n';
        std::cout << "  tg (guided)         = " << t_g << '\n';

        std::cout << "\nAcceleration (with init):\n";
        std::cout << "  Static              = " << a_stp << '\n';
        std::cout << "  Dynamic             = " << a_dp << '\n';
        std::cout << "  Guided              = " << a_gp << '\n';

        std::cout << "\nAcceleration (without init):\n";
        std::cout << "  Static              = " << a_st << '\n';
        std::cout << "  Dynamic             = " << a_d << '\n';
        std::cout << "  Guided              = " << a_g << '\n';

        delete[] a;
        delete[] b;
        delete[] c;
        delete[] d;
        delete[] e;
    }

    return 0;
}
