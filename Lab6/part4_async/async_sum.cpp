#include <iostream>
#include <vector>
#include <future>
#include <chrono>
#include <random>
#include <cmath>
#include <thread>
#include <iomanip>
#include <functional>

// Задана функція (досить "важка", щоб було що розпаралелювати)
double f(double x)
{
    return std::sin(x) * std::cos(x) + std::sqrt(x) + std::log(x + 1.0);
}

// Послідовна версія
double sumSequential(const std::vector<double>& data)
{
    double sum = 0.0;
    for (double x : data)
    {
        sum += f(x);
    }
    return sum;
}

// Сума f(x) для діапазону [begin, end)
double sumRange(const std::vector<double>& data, std::size_t begin, std::size_t end)
{
    double sum = 0.0;
    for (std::size_t i = begin; i < end; ++i)
    {
        sum += f(data[i]);
    }
    return sum;
}

// Паралельна версія: tasks асинхронних задач
double sumParallel(const std::vector<double>& data, int tasks)
{
    std::vector<std::future<double>> futures;
    const std::size_t n = data.size();
    const std::size_t chunk = n / tasks;

    for (int t = 0; t < tasks; ++t)
    {
        std::size_t begin = static_cast<std::size_t>(t) * chunk;
        std::size_t end = (t == tasks - 1) ? n : begin + chunk;

        futures.push_back(std::async(std::launch::async,
                                     sumRange, std::cref(data), begin, end));
    }

    double total = 0.0;
    for (auto& fut : futures)
    {
        total += fut.get();
    }
    return total;
}

// Середній час (мс) виконання функції за кілька повторів
template <typename Func>
double measure(Func func, int repeats, double& result)
{
    double totalMs = 0.0;
    for (int r = 0; r < repeats; ++r)
    {
        auto start = std::chrono::steady_clock::now();
        result = func();
        auto end = std::chrono::steady_clock::now();
        totalMs += std::chrono::duration<double, std::milli>(end - start).count();
    }
    return totalMs / repeats;
}

int main()
{
    const int REPEATS = 5;
    const std::vector<std::size_t> sizes = {1'000'000, 5'000'000, 10'000'000};
    const std::vector<int> taskCounts = {2, 4, 8};

    std::cout << "Hardware threads: "
              << std::thread::hardware_concurrency() << "\n\n";

    std::mt19937 gen(42);
    std::uniform_real_distribution<double> dist(0.0, 100.0);

    std::cout << std::fixed << std::setprecision(3);
    std::cout << std::setw(12) << "Size"
              << std::setw(8)  << "Tasks"
              << std::setw(14) << "T_seq (ms)"
              << std::setw(14) << "T_par (ms)"
              << std::setw(12) << "Speedup"
              << std::setw(12) << "Efficiency"
              << std::setw(10) << "Correct" << '\n';

    for (std::size_t n : sizes)
    {
        std::vector<double> data(n);
        for (auto& x : data) x = dist(gen);

        double seqResult = 0.0;
        double tSeq = measure([&]() { return sumSequential(data); },
                              REPEATS, seqResult);

        for (int k : taskCounts)
        {
            double parResult = 0.0;
            double tPar = measure([&]() { return sumParallel(data, k); },
                                  REPEATS, parResult);

            double speedup = tSeq / tPar;
            double efficiency = speedup / k;
            bool correct = std::abs(seqResult - parResult)
                           <= 1e-9 * std::abs(seqResult);

            std::cout << std::setw(12) << n
                      << std::setw(8)  << k
                      << std::setw(14) << tSeq
                      << std::setw(14) << tPar
                      << std::setw(12) << speedup
                      << std::setw(12) << efficiency
                      << std::setw(10) << (correct ? "yes" : "NO") << '\n';
        }
    }
    return 0;
}