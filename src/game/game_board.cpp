#include "../../include/game/game_board.h"

#include <queue>

#include "../../include/game/game.h"

void GameBoard::draw(HDC hdc, const std::vector<Unit *> &units) {
    // Рисуем сетку
    Constants::hPen = CreatePen(PS_SOLID, 1, COLOR_GRAY);
    SelectObject(hdc, Constants::hPen);

    for (int i = 0; i <= BOARD_SIZE; ++i) {
        MoveToEx(hdc, 50, 50 + i * CELL_SIZE, nullptr);
        LineTo(hdc, 50 + BOARD_SIZE * CELL_SIZE, 50 + i * CELL_SIZE);
        MoveToEx(hdc, 50 + i * CELL_SIZE, 50, nullptr);
        LineTo(hdc, 50 + i * CELL_SIZE, 50 + BOARD_SIZE * CELL_SIZE);
    }

    // Рисуем доступные клетки для перемещения
    Constants::hBrush = CreateSolidBrush(COLOR_LIGHT_BLUE);
    SelectObject(hdc, Constants::hBrush);
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (reachableCells[i][j]) {
                int x = 50 + i * CELL_SIZE;
                int y = 50 + j * CELL_SIZE;
                Rectangle(hdc, x, y, x + CELL_SIZE, y + CELL_SIZE);
            }
        }
    }
    DeleteObject(Constants::hBrush);

    // Рисуем клетки для атаки
    Constants::hBrush = CreateSolidBrush(COLOR_PINK);
    SelectObject(hdc, Constants::hBrush);
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (attackableCells[i][j]) {
                int x = 50 + i * CELL_SIZE;
                int y = 50 + j * CELL_SIZE;
                Rectangle(hdc, x, y, x + CELL_SIZE, y + CELL_SIZE);
            }
        }
    }
    DeleteObject(Constants::hBrush);

    // Рисуем юнитов и полоски здоровья
    HFONT hFont = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             VARIABLE_PITCH, TEXT("Segoe UI Emoji"));
    auto hOldFont = static_cast<HFONT>(SelectObject(hdc, hFont));

    for (const auto &unit: units) {
        if (unit->isAlive()) {
            const auto [fst, snd] = unit->getPosition();
            const int x = 50 + fst * CELL_SIZE;
            const int y = 50 + snd * CELL_SIZE;

            // Рисуем смайлик
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, unit->getColor());
            TextOutW(hdc, x + CELL_SIZE / 2 - 15, y + CELL_SIZE / 2 - 15, unit->getSymbol(), wcslen(unit->getSymbol()));

            // Рисуем полоску здоровья над юнитом
            constexpr int barWidth = CELL_SIZE - 10;
            constexpr int barHeight = 6;
            int barX = x + 5;
            int barY = y + 5;

            // Фон полоски (красный)
            Constants::hBrush = CreateSolidBrush(COLOR_RED);
            RECT healthBarBg = {barX, barY, barX + barWidth, barY + barHeight};
            FillRect(hdc, &healthBarBg, Constants::hBrush);
            DeleteObject(Constants::hBrush);

            // Зеленая часть - текущее здоровье
            if (const int greenWidth = static_cast<int>(static_cast<float>(unit->getHealth()) / unit->getMaxHealth() * barWidth); greenWidth > 0) {
                Constants::hBrush = CreateSolidBrush(COLOR_GREEN);
                RECT healthBarFg = {barX, barY, barX + greenWidth, barY + barHeight};
                FillRect(hdc, &healthBarFg, Constants::hBrush);
                DeleteObject(Constants::hBrush);
            }

            // Рамка полоски
            Constants::hPen = CreatePen(PS_SOLID, 1, COLOR_BLACK);
            const auto hOldPen = static_cast<HPEN>(SelectObject(hdc, Constants::hPen));
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, barX, barY, barX + barWidth, barY + barHeight);
            SelectObject(hdc, hOldPen);
            DeleteObject(Constants::hPen);
        }
    }

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);

    // Рисуем курсор
    int cursorX = 50 + cursorPos.first * CELL_SIZE;
    int cursorY = 50 + cursorPos.second * CELL_SIZE;
    Constants::hPen = CreatePen(PS_SOLID, 2, COLOR_YELLOW);

    SelectObject(hdc, Constants::hPen);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, cursorX, cursorY, cursorX + CELL_SIZE, cursorY + CELL_SIZE);
    DeleteObject(Constants::hPen);
}

void GameBoard::moveCursor(const char direction)  {
    switch (direction) {
        case 'W': if (cursorPos.second > 0) {
                cursorPos.second--;
            }
            break;
        case 'A': if (cursorPos.first > 0) {
                cursorPos.first--;
            }
            break;
        case 'S': if (cursorPos.second < BOARD_SIZE - 1) {
                cursorPos.second++;
            }
            break;
        case 'D': if (cursorPos.first < BOARD_SIZE - 1) {
                cursorPos.first++;
            }
            break;
        case 'Q': if (cursorPos.first <= 0 || cursorPos.second <= 0) {
            } else { cursorPos.first--; cursorPos.second--; }
            break;
        case 'E': if (cursorPos.first >= BOARD_SIZE - 1 || cursorPos.second <= 0) {
            } else { cursorPos.first++; cursorPos.second--; }
            break;
        case 'Z': if (cursorPos.first <= 0 || cursorPos.second >= BOARD_SIZE - 1) {
            } else { cursorPos.first--; cursorPos.second++; }
            break;
        case 'C': if (cursorPos.first >= BOARD_SIZE - 1 || cursorPos.second >= BOARD_SIZE - 1) {
            } else { cursorPos.first++; cursorPos.second++; }
            break;
        default: ;
    }
}

void GameBoard::calculateReachableCells(const Unit *unit, const std::vector<Unit *> &allUnits){
    for (auto& row : reachableCells) std::fill(row.begin(), row.end(), false);

    std::pair<int, int> startPos = unit->getPosition();
    int range = unit->getMovementRange();
    const bool diagonal = unit->canMoveDiagonal();

    std::vector visited(BOARD_SIZE, std::vector(BOARD_SIZE, false));
    std::queue<std::pair<std::pair<int, int>, int>> q;
    q.emplace(startPos, range);
    visited[startPos.first][startPos.second] = true;

    while (!q.empty()) {
        const auto [fst, snd] = q.front(); q.pop();
        const int x = fst.first;
        const int y = fst.second;
        const int remaining = snd;

        if (remaining == 0) continue;

        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                if (!diagonal && dx != 0 && dy != 0) continue;

                int ny = y + dy;
                if (int nx = x + dx; nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && !visited[nx][ny]) {
                    bool cellFree = true;
                    for (const auto& otherUnit : allUnits) {
                        if (otherUnit->isAlive() && otherUnit != unit &&
                           otherUnit->getPosition() == std::make_pair(nx, ny)) {
                            cellFree = false;
                            break;
                           }
                    }

                    if (cellFree) {
                        reachableCells[nx][ny] = true;
                        visited[nx][ny] = true;
                        q.push({{nx, ny}, remaining - 1});
                    }
                }
            }
        }
    }
}

void GameBoard::calculateAttackableCells(const Unit *unit, const std::vector<Unit *> &allUnits) {
    for (auto& row : attackableCells) std::fill(row.begin(), row.end(), false);

    auto [fst, snd] = unit->getPosition();
    const int range = unit->getAttackRange();

    for (int x = 0; x < BOARD_SIZE; ++x) {
        for (int y = 0; y < BOARD_SIZE; ++y) {
            if (const int distance = std::abs(x - fst) + std::abs(y - snd); distance <= range) {
                for (const auto& otherUnit : allUnits) {
                    if (otherUnit->isAlive() && otherUnit->getPlayer() != unit->getPlayer() &&
                        otherUnit->getPosition() == std::make_pair(x, y)) {
                        attackableCells[x][y] = true;
                        break;
                    }
                }
            }
        }
    }
}

void GameBoard::clearReachableCells() {
    for (auto& row : reachableCells) std::fill(row.begin(), row.end(), false);
}

void GameBoard::clearAttackableCells() {
    for (auto& row : attackableCells) std::fill(row.begin(), row.end(), false);
}

bool GameBoard::isCellReachable(const int x, const int y) const {
    return (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) ? reachableCells[x][y] : false;
}

bool GameBoard::isCellAttackable(const int x, const int y) const {
    return (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) ? attackableCells[x][y] : false;
}

std::pair<int, int> GameBoard::getCursorPos() const {
    return cursorPos;
}