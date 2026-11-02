# Лабораторна робота №7. Обробка винятків. Тестування програми

Варіант 2: **нелінійні рівняння** (бібліотека з лабораторної роботи №4).
Мова C++20, CMake, vcpkg, Google Test, CTest.

## Склад проєкту

| Каталог / ціль | Призначення |
|---|---|
| `include/`, `src/` → `RootFindingLib` | статична бібліотека: алгоритми, клас `RootFindingTask`, винятки |
| `demo/` → `RootDemo` | демонстрація помилок, розкручування стека, сильної гарантії |
| `antipattern/` → `AntipatternDemo` | `throw` типів `int` і `std::string`, `throw ex;` проти `throw;` |
| `tests/` → `RootTests` | модульні тести Google Test |

## Збирання і запуск (macOS / Linux)

```bash
export VCPKG_ROOT="$HOME/vcpkg"
cmake --preset mac-debug
cmake --build --preset mac-debug
ctest --preset mac-debug
./out/build/mac-debug/RootDemo
./out/build/mac-debug/AntipatternDemo
```

## Інваріанти класу `RootFindingTask`

1. `a` і `b` скінченні, `a < b`, довжина `b - a` скінченна.
2. `eps` скінченне, `eps > 0`.
3. `maxIter > 0`.
4. Збережені результати або відсутні, або відповідають поточним `a`, `b`, `eps`, `maxIter`
   (після успішної зміни параметрів старі результати скидаються).

Інваріанти перевіряються в конструкторі, `setInterval()` і `setPrecision()` до зміни стану,
тому некоректний об'єкт створити не можна.

## Ієрархія винятків

```
std::runtime_error
└── RootFindingException
    ├── NoSignChangeException    (a, b, f(a), f(b))
    ├── ZeroDerivativeException  (метод, точка, значення)
    └── ConvergenceException     (maxIter, останнє наближення)
```

Стандартні винятки: `std::invalid_argument`, `std::domain_error`, `std::overflow_error`, `std::logic_error`.

| Операція | Умова помилки | Виняток |
|---|---|---|
| конструктор, `setInterval`, `bisection` | `a >= b`, NaN/Inf межі | `std::invalid_argument` |
| `bisection` | однаковий знак `f(a)`, `f(b)` | `NoSignChangeException` |
| `newton` | `\|f'(x)\|` близька до нуля | `ZeroDerivativeException` |
| `secant` | нульовий знаменник `f(x1) - f(x0)` | `ZeroDerivativeException` |
| `bisection`, `newton`, `secant` | точність не досягнута за `maxIter` | `ConvergenceException` |
| усі методи | функція повернула NaN/Inf | `std::domain_error` |
| конструктор | довжина інтервалу не скінченна | `std::overflow_error` |
| усі методи | порожня `std::function`, `eps <= 0`, `maxIter <= 0` | `std::invalid_argument` |
| `results()` | результатів ще немає | `std::logic_error` |

## Гарантії безпеки щодо винятків

| Метод | Гарантія | Механізм |
|---|---|---|
| `RootFindingTask(...)` | об'єкт не створюється при винятку | перевірка в тілі конструктора |
| `setInterval()` | strong | перевірка до зміни, потім присвоєння |
| `setPrecision()` | strong | перевірка до зміни, потім присвоєння |
| `computeRoots()` | strong | обчислення у локальних змінних, запис результатів наприкінці |
| `clearResults()` | no-throw (`noexcept`) | `optional::reset()` |
| геттери, `hasResults()` | no-throw (`noexcept`) | лише читання |

## Розкручування стека: очікувана послідовність (записано ДО запуску)

Ланцюжок `level1() → level2() → level3()`; у кожній функції локальний `ScopeLogger`.
У `level3()` виняток `NoSignChangeException`; `level2()` перехоплює його, виводить
діагностику й виконує `throw;`; `main()` перехоплює остаточно.

Очікувана послідовність:

```
[ENTER] level1
[ENTER] level2
[ENTER] level3
Computing roots on interval without sign change...
[EXIT ] level3
[LOG] level2 caught: No sign change on interval
[EXIT ] level2
[EXIT ] level1
[MAIN] NoSignChangeException: No sign change on interval (f(a)=2, f(b)=7)
Program continues normally.
```

Обґрунтування: деструктори викликаються у порядку, зворотному до створення, і
**до** передачі керування обробнику. `level3` знищується під час виходу з нього,
тому `[EXIT ] level3` з'являється раніше за `[LOG]`. Після `throw;` знищується
`level2`, далі `level1`, і лише тоді обробник у `main()` отримує керування.

**Фактичний результат:** _(вставити після запуску `RootDemo`, Part 2; порівняти з очікуваним)_

## Антипатерни: висновок

Виняток типів `int` і `std::string` не перехоплюється обробником
`catch (const std::exception&)`, його можна зловити лише окремим обробником цього
типу або `catch (...)`, а той не дає доступу ні до типу, ні до значення.
Стандартні та користувацькі класи, похідні від `std::exception`, мають єдиний
інтерфейс `what()`, дозволяють будувати ієрархію й централізовано обробляти
помилки, а власні класи ще й несуть діагностичні дані (межі, значення, номер ітерації).
Тому в бібліотеках слід використовувати класи винятків, а не примітивні типи.
Також слід повторно генерувати виняток через `throw;`, а не `throw ex;`, інакше
виникає object slicing і втрачається похідний тип.

## Негативний самоконтроль

_(заповнюється на наступному кроці)_
