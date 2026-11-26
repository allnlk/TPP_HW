// Частина 3. Чисті функції та незмінність даних
// Варіант 12. Видалити центральний елемент вектора. Якщо кількість елементів
// непарна - видалити один центральний елемент; якщо парна - два центральні.
// Для порожнього вектора повернути порожній вектор.

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <vector>

// Чиста функція: вхідний вектор передається за константним посиланням
// і не змінюється, результат формується в новому векторі.
std::vector<int> removeCenter(const std::vector<int>& source) {
    if (source.empty()) {
        return {};
    }

    const std::size_t n = source.size();
    // Скільки елементів видаляємо: 1 для непарної довжини, 2 для парної
    const std::size_t removeCount = (n % 2 == 0) ? 2 : 1;
    // Індекс першого центрального елемента
    const std::size_t start = (n - removeCount) / 2;

    std::vector<int> result;
    result.reserve(n - removeCount);

    // Копіюємо частину до центру і частину після центру
    std::copy(source.begin(), source.begin() + start, std::back_inserter(result));
    std::copy(source.begin() + start + removeCount, source.end(),
              std::back_inserter(result));

    return result;
}

void printVector(const std::vector<int>& numbers) {
    std::cout << "[ ";
    std::for_each(
        numbers.begin(),
        numbers.end(),
        [](int x) { std::cout << x << ' '; }
    );
    std::cout << "]\n";
}

void demo(const std::vector<int>& source) {
    std::cout << "Source before: ";
    printVector(source);

    std::vector<int> result = removeCenter(source);

    std::cout << "Source after:  ";
    printVector(source);
    std::cout << "Result:        ";
    printVector(result);
    std::cout << '\n';
}

int main() {
    std::cout << "Odd size (7):\n";
    demo({1, 2, 3, 4, 5, 6, 7});

    std::cout << "Even size (6):\n";
    demo({10, 20, 30, 40, 50, 60});

    std::cout << "One element:\n";
    demo({42});

    std::cout << "Two elements:\n";
    demo({8, 9});

    std::cout << "Empty vector:\n";
    demo({});
}
