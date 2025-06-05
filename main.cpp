#include <windows.h>
#include <vector>
#include <ctime>
#include <sstream>

#include "include/constants.h"
#include "include/game/game.h"

// Глобальный объект игры
Game* game = nullptr;

// Оконная процедура
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            GetClientRect(hwnd, &Constants::clientRect);
            game = new Game();
            break;

        case WM_PAINT:
            Constants::hdc = BeginPaint(hwnd, &Constants::ps);
            game->draw(Constants::hdc);
            EndPaint(hwnd, &Constants::ps);
            break;

        case WM_KEYDOWN:
            game->handleKeyPress(wParam);
            break;

        case WM_DESTROY:
            delete game;
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Точка входа
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    srand(static_cast<unsigned int>(time(nullptr)));

    // Изменяем на Unicode версию
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"StrategyGameWindow";

    if (!RegisterClassExW(&wc)) {
        MessageBoxA(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    // Создаем окно
    Constants::hwnd = CreateWindowExW(
        0,
        L"StrategyGameWindow",
        L"Стратегическая игра",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL
    );

    if (Constants::hwnd == NULL) {
        MessageBoxA(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Центрируем окно
    RECT rc;
    GetWindowRect(Constants::hwnd, &rc);
    int xPos = (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2;
    int yPos = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2;
    SetWindowPos(Constants::hwnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

    ShowWindow(Constants::hwnd, nCmdShow);
    UpdateWindow(Constants::hwnd);

    // Цикл сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}