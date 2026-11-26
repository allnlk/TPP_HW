#include <cstddef>
#include <iostream>
#include <ranges>

template <class R>
double average(const R& r) {
    double sum = 0.0;
    std::size_t n = 0;

    for (auto x : r) {
        sum += x;
        ++n;
    }

    return n ? sum / n : 0.0;
}

int main() {
    auto r = std::views::iota(1, 31)
           | std::views::filter(
                [](int x) {
                    return x % 3 == 0;
                }
             )
           | std::views::transform(
                [](int x) {
                    return x * x;
                }
             );

    std::cout << average(r) << std::endl;
}
