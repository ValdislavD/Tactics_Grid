#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <windows.h>

class Constants {
public:
    static HWND hwnd;
    static HDC hdc;
    static PAINTSTRUCT ps;
    static RECT clientRect;
    static HBRUSH hBrush;
    static HPEN hPen;

};

// Глобальные переменные

// Константы
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr int CELL_SIZE = 50;
constexpr int BOARD_SIZE = 10;

// Смайлики для юнитов
inline auto WARRIOR_SYMBOL = L"🤺"; // Воин
inline auto ARCHER_SYMBOL = L"🏹";  // Лучник
inline auto MAGE_SYMBOL = L"🧙";    // Маг

// Цвета
const COLORREF COLOR_RED = RGB(255, 0, 0);
const COLORREF COLOR_GREEN = RGB(0, 255, 0);
const COLORREF COLOR_BLUE = RGB(0, 0, 255);
const COLORREF COLOR_YELLOW = RGB(255, 255, 0);
const COLORREF COLOR_WHITE = RGB(255, 255, 255);
const COLORREF COLOR_BLACK = RGB(0, 0, 0);
const COLORREF COLOR_GRAY = RGB(200, 200, 200);
const COLORREF COLOR_LIGHT_BLUE = RGB(100, 100, 255);
const COLORREF COLOR_PINK = RGB(255, 100, 100);
const COLORREF COLOR_PURPLE = RGB(128, 0, 128);
const COLORREF COLOR_ORANGE = RGB(255, 165, 0);

#endif //CONSTANTS_H