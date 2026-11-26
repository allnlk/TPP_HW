// Частина 5. Завдання 4. Передавання діапазону у функцію
//
// Результат компіляції початкового коду (original/task5_4_original.cpp):
//   error: no matching member function for call to 'begin'
//
// Причина: функція приймає діапазон як const R&. Представлення filter_view
// запам'ятовує (кешує) позицію першого елемента під час першого виклику begin(),
// тобто змінює свій стан. Тому begin() для константного filter_view не існує,
// і цикл for не може обійти такий діапазон.
//
// Виправлення: приймати діапазон як forwarding reference (R&&) з обмеженням
// std::ranges::input_range. Так функція працює з views без копіювання.
// Для порожнього діапазону повертається std::optional<double> без значення.

#include <cstddef>
#include <iostream>
#include <optional>
#include <ranges>
#include <vector>

template <std::ranges::input_range R>
std::optional<double> average(R&& range) {
    double sum = 0.0;
    std::size_t count = 0;

    for (auto&& x : range) {
        sum += static_cast<double>(x);
        ++count;
    }

    if (count == 0) {
        return std::nullopt;
    }

    return sum / static_cast<double>(count);
}

void printAverage(const std::optional<double>& result) {
    if (result) {
        std::cout << *result << '\n';
    } else {
        std::cout << "average does not exist (empty range)\n";
    }
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

    std::cout << "Average of squares of multiples of 3 in [1; 30]: ";
    printAverage(average(r));

    // Перевірка порожнього діапазону: чисел, більших за 100, у [1; 30] немає
    auto empty = std::views::iota(1, 31)
               | std::views::filter([](int x) { return x > 100; });

    std::cout << "Empty range: ";
    printAverage(average(empty));

    // Функція працює і зі звичайним контейнером
    std::vector<int> numbers{2, 4, 6};
    std::cout << "Vector {2, 4, 6}: ";
    printAverage(average(numbers));
}
