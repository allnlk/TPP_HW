// Частина 4. Побудова конвеєрів обробки даних двома способами
// Варіант 12. Вибрати парні числа, упорядкувати їх за зростанням та обчислити
// суму. Для сортування необхідно матеріалізувати результат фільтрації
// в окремому контейнері.

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <ranges>
#include <vector>

struct PipelineResult {
    std::vector<int> sortedEven;  // відібрані та впорядковані парні числа
    int sum;                      // їх сума
};

// Іменована функція-предикат: використовується в обох реалізаціях
bool isEven(int x) {
    return x % 2 == 0;
}

// ---------- А. Класична реалізація (STL) ----------
PipelineResult classicPipeline(const std::vector<int>& numbers) {
    // Етап 1. Фільтрація: std::copy_if копіює парні числа
    // у проміжний контейнер even
    std::vector<int> even;
    std::copy_if(numbers.begin(), numbers.end(),
                 std::back_inserter(even), isEven);

    // Етап 2. Сортування: std::sort працює з проміжним контейнером,
    // початковий вектор не змінюється
    std::sort(even.begin(), even.end());

    // Етап 3. Агрегування: std::accumulate обчислює суму
    // (для порожнього контейнера результат 0)
    int sum = std::accumulate(even.begin(), even.end(), 0);

    return {even, sum};
}

// ---------- Б. Реалізація з std::ranges ----------
PipelineResult rangesPipeline(const std::vector<int>& numbers) {
    // Етап 1. Фільтрація: std::views::filter лише описує відбір і нічого
    // не обчислює. Це "ліниве" представлення без копіювання даних.
    auto evenView = numbers | std::views::filter(isEven);

    // Етап 2. Матеріалізація: відсортувати view неможливо, тому копіюємо
    // його в новий контейнер. Саме тут фактично виконується фільтрація:
    // std::ranges::copy проходить представлення і для кожного елемента
    // викликає isEven.
    std::vector<int> even;
    std::ranges::copy(evenView, std::back_inserter(even));

    // Етап 3. Сортування матеріалізованого контейнера
    std::ranges::sort(even);

    // Етап 4. Агрегування
    int sum = std::accumulate(even.begin(), even.end(), 0);

    return {even, sum};
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

void printResult(const char* title, const PipelineResult& result) {
    std::cout << title << '\n';
    std::cout << "  Sorted even: ";
    printVector(result.sortedEven);
    std::cout << "  Sum: " << result.sum << '\n';
}

void demo(const std::vector<int>& numbers) {
    std::cout << "Source: ";
    printVector(numbers);

    PipelineResult classic = classicPipeline(numbers);
    PipelineResult ranges = rangesPipeline(numbers);

    printResult("A. Classic STL:", classic);
    printResult("B. std::ranges:", ranges);

    std::cout << "Results are equal: "
              << (classic.sortedEven == ranges.sortedEven && classic.sum == ranges.sum
                      ? "yes" : "no")
              << '\n';
    std::cout << "Source after: ";
    printVector(numbers);
    std::cout << '\n';
}

int main() {
    demo({14, 3, 8, -6, 7, 2, 10, 5, 0, -1});

    std::cout << "No even numbers:\n";
    demo({1, 3, 5, 7});

    std::cout << "Empty vector:\n";
    demo({});
}
