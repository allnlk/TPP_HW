#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

// Антипатерн 1: виняток примітивного типу
void throwInteger()
{
    throw 42;
}

// Антипатерн 2: виняток типу std::string
void throwString()
{
    throw std::string("Invalid interval boundaries");
}

// Рекомендований підхід: клас зі стандартної ієрархії
void throwStandardException()
{
    throw std::invalid_argument("Invalid interval boundaries");
}

// Неправильне повторне генерування: throw ex; (object slicing)
void rethrowWithSlicing()
{
    try
    {
        throw std::invalid_argument("Invalid argument");
    }
    catch (const std::exception& ex)
    {
        throw ex;   // створюється нова копія типу std::exception
    }
}

// Правильне повторне генерування: throw;
void rethrowCorrectly()
{
    try
    {
        throw std::invalid_argument("Invalid argument");
    }
    catch (const std::exception&)
    {
        throw;      // той самий об'єкт, тип збережено
    }
}

int main()
{
    std::cout << "=== Example 1: int exception ===\n";
    try
    {
        throwInteger();
    }
    catch (const std::exception& ex)
    {
        std::cout << "Caught by std::exception handler: " << ex.what() << '\n';
    }
    catch (...)
    {
        std::cout << "std::exception handler did NOT catch it; "
                     "caught by catch (...) (type and value unknown)\n";
    }

    std::cout << "\n=== Example 2: std::string exception ===\n";
    try
    {
        throwString();
    }
    catch (const std::exception& ex)
    {
        std::cout << "Caught by std::exception handler: " << ex.what() << '\n';
    }
    catch (...)
    {
        std::cout << "std::exception handler did NOT catch it; "
                     "caught by catch (...) (type and value unknown)\n";
    }

    std::cout << "\n=== Example 3: standard exception ===\n";
    try
    {
        throwStandardException();
    }
    catch (const std::exception& ex)
    {
        std::cout << "Caught std::exception: " << ex.what() << '\n';
    }

    std::cout << "\n=== Example 4: throw ex; (object slicing) ===\n";
    try
    {
        rethrowWithSlicing();
    }
    catch (const std::invalid_argument&)
    {
        std::cout << "Caught invalid_argument\n";
    }
    catch (const std::exception&)
    {
        std::cout << "Caught base std::exception (derived type lost)\n";
    }

    std::cout << "\n=== Example 5: throw; (correct rethrow) ===\n";
    try
    {
        rethrowCorrectly();
    }
    catch (const std::invalid_argument&)
    {
        std::cout << "Caught invalid_argument (derived type preserved)\n";
    }
    catch (const std::exception&)
    {
        std::cout << "Caught base std::exception\n";
    }

    return 0;
}
