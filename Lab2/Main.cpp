#include <windows.h>
#include <math.h>
#include <stdio.h>

BOOL RegClass(WNDPROC, LPCTSTR, UINT);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInstance;
wchar_t szClassName[] = L"GraphClass";

const int N = 100;                       // кількість точок графіка

typedef struct
{
    wchar_t name[20];
    float x[N];
    float y[N];
} FUNC;

// Параметри осей 
const double X_MIN = -2.3, X_MAX = -1.8, X_STEP = 0.1;
const double Y_MIN = 2.4,  Y_MAX = 3.2,  Y_STEP = 0.1;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine, int nCmdShow)
{
    MSG msg; HWND hwnd;
    ::hInstance = hInstance;

    if (!RegClass(WndProc, szClassName, COLOR_WINDOW))
        return FALSE;

    hwnd = CreateWindow(szClassName, L"Графік функції y = exp(-x/2)",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        0, 0, hInstance, NULL);
    if (!hwnd) return FALSE;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

BOOL RegClass(WNDPROC Proc, LPCTSTR szName, UINT brBackground)
{
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.lpfnWndProc = Proc;
    wc.hInstance = hInstance;
    wc.lpszClassName = szName;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(UINT_PTR)(brBackground + 1);
    wc.lpszMenuName = NULL;
    return (RegisterClass(&wc) != 0);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static short cx, cy;
    static FUNC myfunc;

    switch (msg)
    {
    case WM_SIZE:
    {
        cx = LOWORD(lParam);
        cy = HIWORD(lParam);
        return 0;
    }
    case WM_CREATE:
    {
        wcscpy_s(myfunc.name, 20, L"y = exp(-x/2)");
        for (int i = 0; i < N; i++)
        {   // Задаємо функцію y = e^(-x/2) на [X_MIN; X_MAX]
            myfunc.x[i] = (float)(X_MIN + (X_MAX - X_MIN) * i / (N - 1));
            myfunc.y[i] = (float)exp(-myfunc.x[i] / 2.0);
        }
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Область графіка (поля навколо для підписів)
        int left   = cx / 8;
        int right  = cx - cx / 12;
        int top    = cy / 8;
        int bottom = cy - cy / 8;

        // Масштаби
        double dx = (double)(right - left) / (X_MAX - X_MIN);
        double dy = (double)(bottom - top) / (Y_MAX - Y_MIN);

        // Перетворення координат функції в екранні
        auto SX = [&](double x) { return (int)(left + (x - X_MIN) * dx); };
        auto SY = [&](double y) { return (int)(bottom - (y - Y_MIN) * dy); };

        int nx = (int)((X_MAX - X_MIN) / X_STEP + 0.5);
        int ny = (int)((Y_MAX - Y_MIN) / Y_STEP + 0.5);
        wchar_t buf[32];

        SetBkMode(hdc, TRANSPARENT);

        // ---- Сітка (світло-сіра пунктирна) ----
        HPEN hGrid = CreatePen(PS_DOT, 1, RGB(200, 200, 200));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hGrid);
        for (int i = 0; i <= nx; i++)
        {
            int px = SX(X_MIN + i * X_STEP);
            MoveToEx(hdc, px, bottom, NULL);
            LineTo(hdc, px, top);
        }
        for (int j = 0; j <= ny; j++)
        {
            int py = SY(Y_MIN + j * Y_STEP);
            MoveToEx(hdc, left, py, NULL);
            LineTo(hdc, right, py);
        }
        SelectObject(hdc, hOldPen);
        DeleteObject(hGrid);

        // ---- Осі координат (чорні, товщина 2) ----
        HPEN hAxis = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
        hOldPen = (HPEN)SelectObject(hdc, hAxis);
        // вісь ординат
        MoveToEx(hdc, left, bottom + 10, NULL);
        LineTo(hdc, left, top - 15);
        // стрілка осі Y
        MoveToEx(hdc, left, top - 15, NULL);
        LineTo(hdc, left - 4, top - 5);
        MoveToEx(hdc, left, top - 15, NULL);
        LineTo(hdc, left + 4, top - 5);
        // вісь абсцис
        MoveToEx(hdc, left - 10, bottom, NULL);
        LineTo(hdc, right + 15, bottom);
        // стрілка осі X
        MoveToEx(hdc, right + 15, bottom, NULL);
        LineTo(hdc, right + 5, bottom - 4);
        MoveToEx(hdc, right + 15, bottom, NULL);
        LineTo(hdc, right + 5, bottom + 4);

        // ---- Риски та підписи по осі X ----
        SetTextAlign(hdc, TA_CENTER | TA_TOP);
        for (int i = 0; i <= nx; i++)
        {
            double xv = X_MIN + i * X_STEP;
            int px = SX(xv);
            MoveToEx(hdc, px, bottom - 4, NULL);
            LineTo(hdc, px, bottom + 5);
            swprintf_s(buf, 32, L"%.1f", xv);
            TextOut(hdc, px, bottom + 8, buf, (int)wcslen(buf));
        }
        // ---- Риски та підписи по осі Y ----
        SetTextAlign(hdc, TA_RIGHT | TA_TOP);
        for (int j = 0; j <= ny; j++)
        {
            double yv = Y_MIN + j * Y_STEP;
            int py = SY(yv);
            MoveToEx(hdc, left - 5, py, NULL);
            LineTo(hdc, left + 4, py);
            swprintf_s(buf, 32, L"%.1f", yv);
            TextOut(hdc, left - 9, py - 8, buf, (int)wcslen(buf));
        }
        SelectObject(hdc, hOldPen);
        DeleteObject(hAxis);

        // ---- Назви осей і функції ----
        SetTextAlign(hdc, TA_LEFT | TA_TOP);
        TextOut(hdc, right + 20, bottom - 8, L"x", 1);
        TextOut(hdc, left + 10, top - 30, L"y", 1);
        TextOut(hdc, left + 40, top - 30, myfunc.name, (int)wcslen(myfunc.name));

        // ---- Графік функції (синє перо) ----
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
        hOldPen = (HPEN)SelectObject(hdc, hPen);
        MoveToEx(hdc, SX(myfunc.x[0]), SY(myfunc.y[0]), NULL);
        for (int i = 1; i < N; i++)
            LineTo(hdc, SX(myfunc.x[i]), SY(myfunc.y[i]));
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}