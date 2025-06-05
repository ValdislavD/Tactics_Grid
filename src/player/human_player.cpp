#include "../../include/player/human_player.h"
#include <sstream>

HumanPlayer::HumanPlayer(
    const int num
) : statusText("Your turn. Use WASD/QEZC to move, Enter to select."), playerNumber(num) {}

void HumanPlayer::setStatus(const std::string &text) {
    statusText = text;
}

void HumanPlayer::drawStatus(HDC hdc) {
    SetTextColor(hdc, playerNumber == 1 ? COLOR_BLUE : COLOR_RED);
    TextOutA(hdc, 50, 20, statusText.c_str(), static_cast<int>(statusText.length()));

    int yPos = 50 + BOARD_SIZE * CELL_SIZE + 20;

    for (const auto& unit : units) {
        if (unit->isAlive()) {
            std::stringstream ss;

            ss << unit->getName() << " (HP: " << unit->getHealth() << "/" << unit->getMaxHealth() << ")";

            TextOutA(hdc, 50, yPos, ss.str().c_str(), static_cast<int>(ss.str().length()));

            yPos += 20;
        }
    }
}

void HumanPlayer::makeMove(GameBoard &board, Player *enemy) {}

int HumanPlayer::getPlayerNumber() const {
    return playerNumber;
}