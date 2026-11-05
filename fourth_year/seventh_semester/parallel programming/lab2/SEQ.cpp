#include <iostream>
#include <iomanip>
#include <chrono>

const int NUM_VECS = 4;
const int SIZE = 6000000;
const int Q_PARAM = 25;
const int RUNS = 20;
const double SUM = 4.0 * SIZE * Q_PARAM;

int main(int argc, char* argv[]) {
    std::cout << std::fixed << std::setprecision(5);

    float* a = new float[SIZE];
    float* b = new float[SIZE];
    float* c = new float[SIZE];
    float* d = new float[SIZE];

    for (int i = 0; i < SIZE; ++i) {
        a[i] = 1.f;
        b[i] = 1.f;
        c[i] = 1.f;
        d[i] = 1.f;
    }

    double t_s = 0.0, sum_s = 0.0;

    for (int run = 0; run < RUNS; ++run) {

        auto t0 = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < SIZE; ++j) {
            for (int q = 0; q < Q_PARAM; ++q) {
                sum_s += a[j] + b[j] + c[j] + d[j];
            }
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        t_s += std::chrono::duration<double>(t1 - t0).count();
    }

    t_s /= RUNS;
    sum_s /= RUNS;

    std::cout << "Sequential program:\n";
    std::cout << "  Threads           = 1" << '\n';
    std::cout << "  Type              = float\n";
    std::cout << "  NUM_VECS          = " << NUM_VECS << '\n';
    std::cout << "  SIZE              = " << SIZE << '\n';
    std::cout << "  Q_PARAM           = " << Q_PARAM << '\n';
    std::cout << "  RUNS              = " << RUNS << '\n';
    std::cout << "  SUM               = " << SUM << '\n';

    std::cout << "\nTime (sec) (" << RUNS << " runs):\n";
    std::cout << "  ts  (sequential)  = " << t_s << '\n';

    std::cout << "\nChecksums:\n";
    std::cout << "  Expected          = " << SUM << '\n';
    std::cout << "  Sequential        = " << sum_s << '\n';

    delete[] a;
    delete[] b;
    delete[] c;
    delete[] d;

    return 0;
}
