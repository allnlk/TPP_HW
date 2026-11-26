# Лабораторна робота №11 — Основи функціонального програмування в C++ (варіант 12)

C++23, CMake. Кожна частина — окрема програма.

| Частина | Варіант 12 | Файл |
|---|---|---|
| 1. Лямбда-вирази та алгоритми | Максимальний елемент (`std::optional<int>`) | `src/part1_max.cpp` |
| 2. Функтори | `IsInRange` — належність до [a; b] | `src/part2_is_in_range.cpp` |
| 3. Чисті функції | Видалення центрального елемента (1 або 2) | `src/part3_remove_center.cpp` |
| 4. Конвеєри | Парні → сортування → сума (STL і `std::ranges`) | `src/part4_pipeline.cpp` |
| 5.1 | `std::remove_if` | `src/part5_1_remove_if.cpp` |
| 5.2 | Захоплення у лямбдах | `src/part5_2_capture.cpp` |
| 5.3 | `std::ranges::dangling` | `src/part5_3_dangling.cpp` |
| 5.4 | Передавання діапазону (`R&&`) | `src/part5_4_average.cpp` |

Початковий код частини 5 з методички — у папці `original/`.

## Збирання та запуск

    cmake --preset default
    cmake --build build
    ./build/part1_max

## Початкові фрагменти частини 5

    clang++ -std=c++23 original/task5_1_original.cpp -o build/task5_1_original && ./build/task5_1_original
    clang++ -std=c++23 original/task5_2_original.cpp -o build/task5_2_original && ./build/task5_2_original
    clang++ -std=c++23 original/task5_3_original.cpp   # помилка компіляції (std::ranges::dangling)
    clang++ -std=c++23 original/task5_4_original.cpp   # помилка компіляції (begin для const filter_view)
