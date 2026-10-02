#include <windows.h>
#include <math.h>
#include <stdlib.h>

// Ідентифікатори елементів керування
#define ID_EDIT_X  101
#define ID_EDIT_Y  102
#define ID_BUTTON  103
#define ID_RESULT  104

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    // Опис класу вікна
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(INT_PTR)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"Lab3Task1Class";
    RegisterClassExW(&wc);

    // Створення головного вікна
    HWND hwnd = CreateWindowExW(0, L"Lab3Task1Class",
        L"Обчислення функції (варіант 12)",
        WS_OVERLAPPEDWINDOW, 500, 300, 500, 350,
        nullptr, nullptr, hInstance, nullptr);
    if (hwnd == nullptr) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Цикл обробки повідомлень
    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Дескриптори елементів керування
    static HWND hEditX, hEditY, hResult;

    switch (message)
    {
    // Створення дочірніх елементів
    case WM_CREATE:
    {
        HINSTANCE hInst = ((LPCREATESTRUCT)lParam)->hInstance;

        // Поле введення x
        hEditX = CreateWindowExW(0, L"EDIT", L"2",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
            50, 70, 100, 25, hwnd, (HMENU)ID_EDIT_X, hInst, nullptr);

        // Поле введення y
        hEditY = CreateWindowExW(0, L"EDIT", L"4",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
            180, 70, 100, 25, hwnd, (HMENU)ID_EDIT_Y, hInst, nullptr);

        // Кнопка
        CreateWindowExW(0, L"BUTTON", L"Розрахувати",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50, 120, 120, 30, hwnd, (HMENU)ID_BUTTON, hInst, nullptr);

        // Статичне поле для результату
        hResult = CreateWindowExW(0, L"STATIC", L"0",
            WS_CHILD | WS_VISIBLE,
            180, 170, 250, 25, hwnd, (HMENU)ID_RESULT, hInst, nullptr);
        break;
    }
    // Обробка команд від елементів керування
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == ID_BUTTON)
        {
            wchar_t bufferX[50] = {};
            wchar_t bufferY[50] = {};

            // Отримання тексту з полів
            GetWindowTextW(hEditX, bufferX, 50);
            GetWindowTextW(hEditY, bufferY, 50);

            // Перетворення тексту в числа
            double x = _wtof(bufferX);
            double y = _wtof(bufferY);

            if (x + 2 == 0)
            {
                SetWindowTextW(hResult, L"Помилка: ділення на нуль");
            }
            else
            {
                // f(x, y) = [ x * (y + 2) / (x + 2) ], ціла частина
                int f = (int)floor(x * (y + 2) / (x + 2));

                wchar_t result[50];
                wsprintfW(result, L"%d", f);
                SetWindowTextW(hResult, result);
            }
        }
        break;
    }
    // Підписи у вікні
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        TextOutW(hdc, 50, 35, L"Введіть x та y:", 15);
        TextOutW(hdc, 50, 170, L"Результат:", 10);
        EndPaint(hwnd, &ps);
        break;
    }
    // Закриття вікна
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
    return 0;
}