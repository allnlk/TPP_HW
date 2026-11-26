// Частина 6. Завдання підвищеної складності
// Варіант 3. Аналіз продажів.
// - відібрати товари, ціна яких перевищує 50;
// - обчислити дохід від кожної такої позиції (ціна * кількість);
// - обчислити загальний дохід чистою функцією total_income(...).
// Відбір і перетворення - std::views::filter та std::views::transform.

#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

struct Sale {
    std::string product;
    double price;
    int quantity;
};

// Іменовані чисті функції: використовуються в конвеєрі
bool isExpensive(const Sale& sale) {
    return sale.price > 50;
}

double income(const Sale& sale) {
    return sale.price * sale.quantity;
}

// Чиста функція агрегування. Діапазон приймається як R&& (а не const R&),
// щоб функція працювала і з контейнерами, і з views. Для порожнього
// діапазону загальний дохід дорівнює 0.
template <std::ranges::input_range R>
double total_income(R&& incomes) {
    return std::accumulate(std::ranges::begin(incomes),
                           std::ranges::end(incomes),
                           0.0);
}

void analyze(const std::vector<Sale>& sales) {
    // Конвеєр: views лише описують обчислення, дані не копіюються
    // і початковий вектор не змінюється
    auto expensive = sales | std::views::filter(isExpensive);
    auto incomes = expensive | std::views::transform(income);

    double total = total_income(incomes);

    // Виведення - окремо від обчислень
    std::cout << "All sales:\n";
    for (const Sale& s : sales) {
        std::cout << "  " << s.product << ": price " << s.price
                  << ", quantity " << s.quantity << '\n';
    }

    std::cout << "Products with price > 50 and income:\n";
    for (const Sale& s : expensive) {
        std::cout << "  " << s.product << ": " << income(s) << '\n';
    }

    std::cout << "Total income: " << total << "\n\n";
}

int main() {
    std::vector<Sale> sales{
        {"Keyboard", 45.0, 10},
        {"Monitor", 220.0, 3},
        {"Mouse", 25.5, 20},
        {"Headphones", 80.0, 5},
        {"Webcam", 55.5, 4},
        {"USB cable", 9.9, 30}
    };

    analyze(sales);

    std::cout << "No products with price > 50:\n";
    analyze({{"Pen", 2.5, 100}, {"Notebook", 12.0, 15}});

    std::cout << "Empty vector:\n";
    analyze({});
}
