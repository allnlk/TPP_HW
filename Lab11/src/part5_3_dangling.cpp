// Частина 5. Завдання 3. Час життя тимчасових об'єктів і std::ranges::dangling
//
// Результат компіляції початкового коду (original/task5_3_original.cpp):
//   error: indirection requires pointer operand
//   ('borrowed_iterator_t<vector<int>>' (aka 'std::ranges::dangling') invalid)
//
// Причина: make() повертає тимчасовий вектор, який знищується одразу після
// виконання рядка з max_element. Ітератор на його елемент став би "висячим".
// Тому std::ranges::max_element для тимчасового контейнера повертає не ітератор,
// а спеціальний тип std::ranges::dangling, який не можна розіменувати. Так
// помилку виявляє компілятор, а не програма під час роботи.
//
// Виправлення: зберегти вектор у змінній, щоб він жив, поки використовується
// ітератор, або одразу отримати значення через std::ranges::max.

#include <algorithm>
#include <iostream>
#include <ranges>
#include <vector>

std::vector<int> make() {
    return {3, 1, 4};
}

int main() {
    // Виправлення 1: вектор зберігається в змінній
    std::vector<int> data = make();
    auto it = std::ranges::max_element(data);

    if (it != data.end()) {
        std::cout << "max_element: " << *it << '\n';
    }

    // Виправлення 2: std::ranges::max повертає саме значення,
    // а не ітератор, тому тимчасовий вектор тут не проблема
    // (вектор не повинен бути порожнім)
    int maxValue = std::ranges::max(make());
    std::cout << "ranges::max: " << maxValue << '\n';
}
