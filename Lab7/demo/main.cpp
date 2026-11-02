#include "RootFinding.h"
#include "RootFindingExceptions.h"
#include "RootFindingTask.h"
#include "ScopeLogger.h"

#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace
{
    double f(double x)  { return x * x - 2.0; }
    double df(double x) { return 2.0 * x; }
    double constF(double)  { return 1.0; }
    double nanF(double)    { return std::numeric_limits<double>::quiet_NaN(); }
}

// ---------------------------------------------------------------
// Частина 1. П'ять сценаріїв помилок (+ domain_error для NaN)
// Обробники розташовано від найбільш спеціалізованих до загальних.
// ---------------------------------------------------------------
void runScenario(const std::string& title, const std::function<void()>& action)
{
    std::cout << "- " << title << ": ";
    try
    {
        action();
        std::cout << "OK\n";
    }
    catch (const NoSignChangeException& ex)
    {
        std::cout << "NoSignChangeException: " << ex.what()
                  << " [a=" << ex.a() << ", b=" << ex.b()
                  << ", f(a)=" << ex.fa() << ", f(b)=" << ex.fb() << "]\n";
    }
    catch (const ZeroDerivativeException& ex)
    {
        std::cout << "ZeroDerivativeException: " << ex.what()
                  << " [method=" << ex.method()
                  << ", point=" << ex.point()
                  << ", value=" << ex.value() << "]\n";
    }
    catch (const ConvergenceException& ex)
    {
        std::cout << "ConvergenceException: " << ex.what()
                  << " [maxIter=" << ex.maxIter()
                  << ", last=" << ex.lastApproximation() << "]\n";
    }
    catch (const RootFindingException& ex)
    {
        std::cout << "RootFindingException: " << ex.what() << '\n';
    }
    catch (const std::invalid_argument& ex)
    {
        std::cout << "std::invalid_argument: " << ex.what() << '\n';
    }
    catch (const std::domain_error& ex)
    {
        std::cout << "std::domain_error: " << ex.what() << '\n';
    }
    catch (const std::exception& ex)
    {
        std::cout << "std::exception: " << ex.what() << '\n';
    }
}

void demoScenarios()
{
    std::cout << "=== Part 1: error scenarios ===\n";

    runScenario("invalid interval (a > b)", [] {
        RootFindingTask task(2.0, 0.0, 1e-9, 100);
    });
    runScenario("no sign change (bisection)", [] {
        bisection(f, 2.0, 3.0, 1e-9, 100);
    });
    runScenario("zero derivative (newton)", [] {
        newton(f, df, 0.0, 1e-9, 50);
    });
    runScenario("zero denominator (secant)", [] {
        secant(constF, 1.0, 2.0, 1e-9, 50);
    });
    runScenario("no convergence (bisection, 3 iterations)", [] {
        bisection(f, 0.0, 2.0, 1e-15, 3);
    });
    runScenario("function returned NaN", [] {
        bisection(nanF, 0.0, 1.0, 1e-9, 100);
    });
    runScenario("correct call", [] {
        bisection(f, 0.0, 2.0, 1e-9, 100);
    });
}

// ---------------------------------------------------------------
// Частина 2. Ланцюжок із трьох функцій: розкручування стека
// і повторне генерування винятку через throw;
// ---------------------------------------------------------------

// Рівень 3: виняток виникає тут
void level3()
{
    ScopeLogger logger("level3");
    std::cout << "Computing roots on interval without sign change...\n";

    RootFindingTask task(2.0, 3.0, 1e-9, 100);
    task.computeRoots(f, df, 2.5);   // кидає NoSignChangeException
}

// Рівень 2: перехоплення, діагностика, повторне генерування
void level2()
{
    ScopeLogger logger("level2");
    try
    {
        level3();
    }
    catch (const RootFindingException& ex)
    {
        std::cout << "[LOG] level2 caught: " << ex.what() << '\n';
        throw;   // той самий об'єкт винятку, без slicing
    }
}

// Рівень 1: лише викликає наступний рівень
void level1()
{
    ScopeLogger logger("level1");
    level2();
}

void demoUnwinding()
{
    std::cout << "\n=== Part 2: stack unwinding and rethrow ===\n";
    try
    {
        level1();
    }
    catch (const NoSignChangeException& ex)
    {
        // Фактичний тип збережено завдяки throw;
        std::cout << "[MAIN] NoSignChangeException: " << ex.what()
                  << " (f(a)=" << ex.fa() << ", f(b)=" << ex.fb() << ")\n";
    }
    catch (const std::exception& ex)
    {
        std::cout << "[MAIN] std::exception: " << ex.what() << '\n';
    }
    std::cout << "Program continues normally.\n";
}

// ---------------------------------------------------------------
// Частина 3. Сильна гарантія: стан після невдалої операції
// ---------------------------------------------------------------
void printState(const RootFindingTask& task)
{
    std::cout << "  interval = [" << task.a() << ", " << task.b() << "]";
    if (task.hasResults())
    {
        std::cout << ", bisection = " << task.results().bisectionRoot
                  << ", newton = " << task.results().newtonRoot;
    }
    else
    {
        std::cout << ", no results";
    }
    std::cout << '\n';
}

void demoStrongGuarantee()
{
    std::cout << "\n=== Part 3: strong guarantee ===\n";

    RootFindingTask task(0.0, 2.0, 1e-9, 100);
    task.computeRoots(f, df, 1.0);

    std::cout << "Before failed operation:\n";
    printState(task);

    try
    {
        // Бісекція вдасться, Ньютон кине виняток (df(0) = 0)
        task.computeRoots(f, df, 0.0);
    }
    catch (const ZeroDerivativeException& ex)
    {
        std::cout << "Exception: " << ex.what() << '\n';
    }

    std::cout << "After failed operation:\n";
    printState(task);
}

int main()
{
    demoScenarios();
    demoUnwinding();
    demoStrongGuarantee();
    return 0;
}
