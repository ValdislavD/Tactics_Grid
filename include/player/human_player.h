#ifndef HUMAN_PLAYER_H
#define HUMAN_PLAYER_H

#include "player.h"

#include <string>

// Реальный игрок
class HumanPlayer final : public Player {
    std::string statusText;
    int playerNumber;

public:
    explicit HumanPlayer(int num);

    void setStatus(const std::string& text);
    void drawStatus(HDC hdc);
    void makeMove(GameBoard& board, Player* enemy) override;
    [[nodiscard]] int getPlayerNumber() const;
};

#endif //HUMAN_PLAYER_H
