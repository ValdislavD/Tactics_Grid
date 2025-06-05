#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include <vector>

#include "../constants.h"
#include "../unit/unit.h"

// Класс игрового поля
class GameBoard {
    std::pair<int, int> cursorPos;
    std::vector<std::vector<bool>> reachableCells;
    std::vector<std::vector<bool>> attackableCells;

public:
    GameBoard() : cursorPos({0, 0}) {
        reachableCells.resize(BOARD_SIZE, std::vector(BOARD_SIZE, false));
        attackableCells.resize(BOARD_SIZE, std::vector(BOARD_SIZE, false));
    }

    void draw(HDC hdc, const std::vector<Unit*>& units);
    void moveCursor(char direction);
    void calculateReachableCells(const Unit* unit, const std::vector<Unit*>& allUnits);
    void calculateAttackableCells(const Unit* unit, const std::vector<Unit*>& allUnits);
    void clearReachableCells();
    void clearAttackableCells();
    [[nodiscard]] bool isCellReachable(int x, int y) const;
    [[nodiscard]] bool isCellAttackable(int x, int y) const;
    [[nodiscard]] std::pair<int, int> getCursorPos() const;
};

#endif //GAME_BOARD_H