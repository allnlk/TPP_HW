// Частина 1. Лямбда-вирази та стандартні алгоритми
// Варіант 12. Знайти максимальний елемент вектора.
// Якщо вектор порожній, результат повинен бути відсутнім (std::optional<int>).

#include <algorithm>
#include <iostream>
#include <optional>
#include <vector>

// Чиста функція: не змінює вектор і нічого не виводить.
// std::max_element обрано тому, що це стандартний алгоритм саме для пошуку
// найбільшого елемента: він один раз проходить діапазон і повертає ітератор
// на максимум. Для порожнього діапазону він повертає end(), тому результат
// загортаємо в std::optional.
std::optional<int> findMax(const std::vector<int>& numbers) {
    auto it = std::max_element(
        numbers.begin(),
        numbers.end(),
        [](int a, int b) { return a < b; }  // критерій порівняння - лямбда
    );

    if (it == numbers.end()) {
        return std::nullopt;
    }

    return *it;
}

// Виведення вектора через std::for_each (вимога методички):
// алгоритм застосовує лямбду до кожного елемента.
void printVector(const std::vector<int>& numbers) {
    std::for_each(
        numbers.begin(),
        numbers.end(),
        [](int x) { std::cout << x << ' '; }
    );
    std::cout << '\n';
}

void printMax(const std::optional<int>& maximum) {
    if (maximum.has_value()) {
        std::cout << "Max element: " << *maximum << '\n';
    } else {
        std::cout << "Max element does not exist (empty vector)\n";
    }
}

int main() {
    std::vector<int> numbers{7, -3, 15, 2, 15, 9, -20};
    std::vector<int> empty;

    std::cout << "Vector: ";
    printVector(numbers);
    printMax(findMax(numbers));

    std::cout << "\nEmpty vector: ";
    printVector(empty);
    printMax(findMax(empty));
}
