#ifndef PLAYER_H
#define PLAYER_H

#include "../unit/unit.h"
#include "../game/game_board.h"

// Игрок
class Player {
protected:
    std::vector<Unit*> units;

public:
    virtual ~Player() {
        for (const auto unit : units) delete unit;
    }

    void addUnit(Unit* unit);
    [[nodiscard]] const std::vector<Unit*>& getUnits() const;
    virtual void makeMove(GameBoard& board, Player* enemy) = 0;
    [[nodiscard]] bool hasAliveUnits() const;
    void removeDeadUnits();
};

#endif //PLAYER_H