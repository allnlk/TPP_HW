#include <windows.h>
#include <math.h>

// Ідентифікатори пунктів меню
#define ID_IMG_SCENE   201
#define ID_TR_COLOR    301
#define ID_TR_ZOOM     302
#define ID_INFO_ABOUT  401
#define ID_INFO_EXIT   402

const double PI = 3.14159265358979;

// Стан програми
bool     g_shown = false;               // чи виведено рисунок
bool     g_night = false;               // день / ніч 
int      g_boatShift = 0;               // зсув човнів
double   g_scale = 1.0;                 // масштаб човнів (збільшення)
COLORREF g_hull = RGB(139, 69, 19);     // колір корпусу човнів

// Масштабування координати
static int S(double k, int v) { return (int)floor(v * k + 0.5); }

// Клас: Море
class Sea
{
public:
    bool night = false;
    int w = 0, h = 0;

    void show(HDC dc, int X, int Y)
    {
        HBRUSH brush = CreateSolidBrush(night ? RGB(0, 30, 90) : RGB(0, 120, 220));
        HGDIOBJ oldBrush = SelectObject(dc, brush);
        HGDIOBJ oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
        Rectangle(dc, X, Y, X + w + 1, Y + h + 1);
        SelectObject(dc, oldPen);
        SelectObject(dc, oldBrush);
        DeleteObject(brush);

        HPEN pen = CreatePen(PS_SOLID, 2, night ? RGB(90, 120, 190) : RGB(255, 255, 255));
        oldPen = SelectObject(dc, pen);
        int rowStep = (h - 30) / 5;
        for (int row = 0; row < 5; row++)
        {
            int yy = Y + 30 + row * rowStep;
            for (int x = X + (row % 2) * 25; x < X + w - 30; x += 50)
            {
                MoveToEx(dc, x, yy, NULL);
                LineTo(dc, x + 10, yy - 5);
                LineTo(dc, x + 20, yy);
                LineTo(dc, x + 30, yy - 5);
            }
        }
        SelectObject(dc, oldPen);
        DeleteObject(pen);
    }
};

// Клас: Сонце (вночі - місяць)
class Sun
{
public:
    bool night = false;

    void show(HDC dc, int X, int Y)
    {
        COLORREF body = night ? RGB(235, 235, 235) : RGB(255, 255, 0);
        COLORREF edge = night ? RGB(160, 160, 160) : RGB(255, 160, 0);

        HPEN pen = CreatePen(PS_SOLID, 3, edge);
        HGDIOBJ oldPen = SelectObject(dc, pen);

        if (!night)
        {
            for (int i = 0; i < 8; i++)
            {
                double a = i * PI / 4;
                MoveToEx(dc, X + (int)(60 * cos(a)), Y + (int)(60 * sin(a)), NULL);
                LineTo(dc, X + (int)(85 * cos(a)), Y + (int)(85 * sin(a)));
            }
        }

        HBRUSH brush = CreateSolidBrush(body);
        HGDIOBJ oldBrush = SelectObject(dc, brush);
        Ellipse(dc, X - 45, Y - 45, X + 45, Y + 45);
        SelectObject(dc, oldBrush);
        DeleteObject(brush);

        if (night)
        {
            HPEN skyPen = CreatePen(PS_SOLID, 1, RGB(10, 10, 60));
            HBRUSH skyBrush = CreateSolidBrush(RGB(10, 10, 60));
            SelectObject(dc, skyPen);
            oldBrush = SelectObject(dc, skyBrush);
            Ellipse(dc, X - 20, Y - 55, X + 70, Y + 35);
            SelectObject(dc, oldBrush);
            SelectObject(dc, pen);
            DeleteObject(skyBrush);
            DeleteObject(skyPen);
        }

        SelectObject(dc, oldPen);
        DeleteObject(pen);
    }
};


// Клас: Хмара
class Cloud
{
public:
    bool night = false;

    void show(HDC dc, int X, int Y)
    {
        COLORREF c = night ? RGB(90, 90, 120) : RGB(255, 255, 255);
        HBRUSH brush = CreateSolidBrush(c);
        HPEN pen = CreatePen(PS_SOLID, 1, c);
        HGDIOBJ oldBrush = SelectObject(dc, brush);
        HGDIOBJ oldPen = SelectObject(dc, pen);

        Ellipse(dc, X - 60, Y - 10, X, Y + 25);
        Ellipse(dc, X - 30, Y - 30, X + 30, Y + 20);
        Ellipse(dc, X, Y - 15, X + 60, Y + 25);
        Ellipse(dc, X - 45, Y, X + 45, Y + 30);

        SelectObject(dc, oldPen);
        SelectObject(dc, oldBrush);
        DeleteObject(pen);
        DeleteObject(brush);
    }
};


// Клас: Човен (k - масштаб, hullColor - колір корпусу)
class Boat
{
public:
    void show(HDC dc, int X, int Y, double k, COLORREF hullColor)
    {
        HBRUSH brush;
        HGDIOBJ oldBrush;

        // Щогла
        MoveToEx(dc, X, Y - S(k, 20), NULL);
        LineTo(dc, X, Y - S(k, 130));

        // Корпус
        POINT hull[4] =
        {
            { X - S(k, 60), Y - S(k, 20) },
            { X + S(k, 60), Y - S(k, 20) },
            { X + S(k, 40), Y },
            { X - S(k, 40), Y }
        };
        brush = CreateSolidBrush(hullColor);
        oldBrush = SelectObject(dc, brush);
        Polygon(dc, hull, 4);
        SelectObject(dc, oldBrush);
        DeleteObject(brush);

        // Велике вітрило
        POINT sail1[3] =
        {
            { X + S(k, 5),  Y - S(k, 125) },
            { X + S(k, 5),  Y - S(k, 25) },
            { X + S(k, 60), Y - S(k, 25) }
        };
        brush = CreateSolidBrush(RGB(255, 255, 255));
        oldBrush = SelectObject(dc, brush);
        Polygon(dc, sail1, 3);
        SelectObject(dc, oldBrush);
        DeleteObject(brush);

        // Мале вітрило (штрихування)
        POINT sail2[3] =
        {
            { X - S(k, 5),  Y - S(k, 110) },
            { X - S(k, 5),  Y - S(k, 25) },
            { X - S(k, 45), Y - S(k, 25) }
        };
        brush = CreateHatchBrush(HS_DIAGCROSS, RGB(220, 0, 0));
        oldBrush = SelectObject(dc, brush);
        Polygon(dc, sail2, 3);
        SelectObject(dc, oldBrush);
        DeleteObject(brush);

        // Прапорець
        POINT flag[3] =
        {
            { X,            Y - S(k, 130) },
            { X,            Y - S(k, 115) },
            { X + S(k, 22), Y - S(k, 122) }
        };
        brush = CreateSolidBrush(RGB(255, 0, 0));
        oldBrush = SelectObject(dc, brush);
        Polygon(dc, flag, 3);
        SelectObject(dc, oldBrush);
        DeleteObject(brush);

        // Ілюмінатори
        brush = CreateSolidBrush(RGB(173, 216, 230));
        oldBrush = SelectObject(dc, brush);
        for (int dx = -28; dx <= 28; dx += 28)
            Ellipse(dc, X + S(k, dx - 7), Y - S(k, 16),
                        X + S(k, dx + 7), Y - S(k, 4));
        SelectObject(dc, oldBrush);
        DeleteObject(brush);
    }
};


// Малювання сцени
void DrawScene(HDC dc, int width, int height)
{
    if (!g_shown)
    {
        RECT all = { 0, 0, width, height };
        FillRect(dc, &all, (HBRUSH)GetStockObject(WHITE_BRUSH));
        return;
    }

    int horizon = height * 55 / 100;

    // Небо
    HBRUSH brush = CreateSolidBrush(g_night ? RGB(10, 10, 60) : RGB(135, 206, 250));
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    HGDIOBJ oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
    Rectangle(dc, 0, 0, width + 1, horizon + 1);
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(brush);

    // Сонце / місяць
    Sun sun;
    sun.night = g_night;
    sun.show(dc, width - 120, 100);

    // Хмари (3 шт.)
    Cloud cloud;
    cloud.night = g_night;
    cloud.show(dc, width * 20 / 100, 90);
    cloud.show(dc, width * 45 / 100, 55);
    cloud.show(dc, width * 62 / 100, 130);

    // Море
    Sea sea;
    sea.night = g_night;
    sea.w = width;
    sea.h = height - horizon;
    sea.show(dc, 0, horizon);

    // Човни (2 шт.)
    Boat boat;
    boat.show(dc, width * 30 / 100 + g_boatShift, horizon + 80, g_scale, g_hull);
    boat.show(dc, width * 70 / 100 + g_boatShift,
              horizon + (height - horizon) * 70 / 100, g_scale, g_hull);
}

// Створення меню
HMENU BuildMenu()
{
    HMENU bar = CreateMenu();

    HMENU mImg = CreatePopupMenu();
    AppendMenuW(mImg, MF_STRING, ID_IMG_SCENE, L"Рисунок: Море");

    HMENU mTr = CreatePopupMenu();
    AppendMenuW(mTr, MF_STRING, ID_TR_COLOR, L"Зміна кольору");
    AppendMenuW(mTr, MF_STRING, ID_TR_ZOOM,  L"Збільшення");

    HMENU mInfo = CreatePopupMenu();
    AppendMenuW(mInfo, MF_STRING, ID_INFO_ABOUT, L"Про програму");
    AppendMenuW(mInfo, MF_STRING, ID_INFO_EXIT,  L"Вихід");

    AppendMenuW(bar, MF_POPUP, (UINT_PTR)mImg,  L"Зображення");
    AppendMenuW(bar, MF_POPUP, (UINT_PTR)mTr,   L"Трансформації");
    AppendMenuW(bar, MF_POPUP, (UINT_PTR)mInfo, L"Інформація");
    return bar;
}


// Віконна процедура
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_IMG_SCENE:               // вивести рисунок
            g_shown = true;
            InvalidateRect(hwnd, nullptr, FALSE);
            break;

        case ID_TR_COLOR:                // зміна кольору корпусів човнів
            g_hull = RGB(255, 0, 0);
            InvalidateRect(hwnd, nullptr, FALSE);
            break;

        case ID_TR_ZOOM:                 // індивідуальна дія: збільшення
            g_scale *= 1.25;
            InvalidateRect(hwnd, nullptr, FALSE);
            break;

        case ID_INFO_ABOUT:
            MessageBoxW(hwnd, L"Лабораторна робота № 3. Варіант 12.",
                        L"Про програму", MB_OK);
            break;

        case ID_INFO_EXIT:
            DestroyWindow(hwnd);
            break;

        default:
            return DefWindowProcW(hwnd, message, wParam, lParam);
        }
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);
        DrawScene(dc, rect.right, rect.bottom);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;

    case WM_LBUTTONDOWN:                
        g_night = !g_night;
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;

    case WM_KEYDOWN:                     
        if (wParam == VK_ESCAPE)
            DestroyWindow(hwnd);
        else if (wParam == VK_LEFT)
        {
            if (g_boatShift > -200) g_boatShift -= 20;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        else if (wParam == VK_RIGHT)
        {
            if (g_boatShift < 200) g_boatShift += 20;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;

    case WM_SIZE:
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}


// Головна функція
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Lab3Task2Class";

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(INT_PTR)(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;
    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(0, CLASS_NAME,
        L"Лабораторна 3. Завдання 2 (варіант 12)",
        WS_OVERLAPPEDWINDOW, 100, 100, 900, 650,
        nullptr, BuildMenu(), hInstance, nullptr);
    if (hwnd == nullptr) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}