#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

std::atomic<int> counter{0};
const int ITERATIONS = 12 * 100000;

void increment()
{
    for (int i = 0; i < ITERATIONS; ++i)
    {
        ++counter;
    }
}

int main()
{
    const int expected = 2 * ITERATIONS;

    for (int run = 1; run <= 5; ++run)
    {
        counter = 0;
        auto start = std::chrono::steady_clock::now();

        std::thread t1(increment);
        std::thread t2(increment);
        t1.join();
        t2.join();

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        std::cout << "Run " << run << ": expected " << expected
                  << ", actual " << counter.load()
                  << ", time " << elapsed.count() << " ms\n";
    }
    return 0;
}