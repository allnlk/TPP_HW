// Частина 5. Завдання 1. Особливості роботи std::remove_if
//
// Результат початкового коду (original/task5_1_original.cpp):
//   5
//   1 3 5 -4 5
//
// Причина: std::remove_if не видаляє елементи з контейнера - алгоритм не має
// доступу до самого вектора, лише до ітераторів. Він переносить елементи, які
// треба залишити, на початок і повертає ітератор на "нову межу". Розмір
// вектора не змінюється, а елементи після цієї межі мають невизначені значення.
//
// Виправлення: результат std::remove_if передати в v.erase(...)
// (ідіома erase-remove) або використати std::erase_if (C++20).

#include <algorithm>
#include <iostream>
#include <vector>

void printVector(const std::vector<int>& v) {
    std::cout << "size = " << v.size() << ": ";
    std::for_each(v.begin(), v.end(), [](int x) { std::cout << x << ' '; });
    std::cout << '\n';
}

int main() {
    auto isNegative = [](int x) { return x < 0; };

    // Початкова поведінка: лише std::remove_if
    std::vector<int> original{1, -2, 3, -4, 5};
    std::remove_if(original.begin(), original.end(), isNegative);
    std::cout << "Only remove_if:     ";
    printVector(original);

    // Виправлення 1: ідіома erase-remove
    std::vector<int> v1{1, -2, 3, -4, 5};
    auto newEnd = std::remove_if(v1.begin(), v1.end(), isNegative);
    v1.erase(newEnd, v1.end());
    std::cout << "erase + remove_if:  ";
    printVector(v1);

    // Виправлення 2: std::erase_if (C++20) робить те саме одним викликом
    std::vector<int> v2{1, -2, 3, -4, 5};
    std::erase_if(v2, isNegative);
    std::cout << "std::erase_if:      ";
    printVector(v2);
}
