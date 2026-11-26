// Частина 2. Функціональні об'єкти (функтори)
// Варіант 12. IsInRange - функтор, який перевіряє, чи належить передане число
// замкненому інтервалу [a; b].

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// Функтор без стану: межі a і b задаються один раз у конструкторі
// і далі не змінюються, тому operator() оголошено як const.
class IsInRange {
private:
    int a;
    int b;

public:
    IsInRange(int lower, int upper)
        : a(lower), b(upper) {}

    bool operator()(int x) const {
        return x >= a && x <= b;
    }
};

void printVector(const std::vector<int>& numbers) {
    std::for_each(
        numbers.begin(),
        numbers.end(),
        [](int x) { std::cout << x << ' '; }
    );
    std::cout << '\n';
}

int main() {
    std::vector<int> numbers{-5, 0, 3, 10, 12, 7, 20, 5};
    int a = 0;
    int b = 10;

    std::cout << "Vector: ";
    printVector(numbers);
    std::cout << "Interval: [" << a << "; " << b << "]\n";

    // ---------- 1. Функтор ----------
    IsInRange inRange(a, b);

    std::cout << "\n--- Functor IsInRange ---\n";
    std::cout << "inRange(5) = " << inRange(5) << '\n';
    std::cout << "inRange(12) = " << inRange(12) << '\n';

    // std::count_if підраховує елементи, для яких функтор повертає true
    auto countFunctor = std::count_if(numbers.begin(), numbers.end(), inRange);

    // std::copy_if відбирає ці елементи в новий вектор
    std::vector<int> selectedFunctor;
    std::copy_if(numbers.begin(), numbers.end(),
                 std::back_inserter(selectedFunctor), inRange);

    std::cout << "Count in range: " << countFunctor << '\n';
    std::cout << "Elements in range: ";
    printVector(selectedFunctor);

    // ---------- 2. Еквівалентний лямбда-вираз ----------
    // Межі захоплюються за значенням [a, b] - так само, як функтор
    // зберігає їх у полях. Компілятор сам створює клас з operator().
    auto inRangeLambda = [a, b](int x) {
        return x >= a && x <= b;
    };

    std::cout << "\n--- Lambda ---\n";
    auto countLambda = std::count_if(numbers.begin(), numbers.end(), inRangeLambda);

    std::vector<int> selectedLambda;
    std::copy_if(numbers.begin(), numbers.end(),
                 std::back_inserter(selectedLambda), inRangeLambda);

    std::cout << "Count in range: " << countLambda << '\n';
    std::cout << "Elements in range: ";
    printVector(selectedLambda);

    std::cout << "\nResults are equal: "
              << (countFunctor == countLambda && selectedFunctor == selectedLambda
                      ? "yes" : "no")
              << '\n';

    // Порівняння підходів:
    // - функтор - іменований клас: його можна повторно використовувати в різних
    //   місцях програми, додавати методи та документувати;
    // - лямбда коротша і пишеться прямо в місці використання, але для
    //   повторного використання її доводиться зберігати у змінній.
    // Результат роботи обох способів однаковий.
}
