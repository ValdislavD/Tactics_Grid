#ifndef GAME_H
#define GAME_H

#include <sstream>

#include "game_board.h"
#include "../player/player.h"
#include "../player/human_player.h"
#include "../factory/unit_factory.h"

enum GameState {
    MAIN_MENU,
    GAME_PLAY,
    GAME_OVER,
    HELP_SCREEN
};

class Game {
    GameState currentState;
    GameBoard board;
    std::vector<Player*> players;
    int currentPlayerIndex;
    Unit* selectedUnit;
    bool isSelectingTarget;
    bool isMovingUnit;
    int selectedMenuItem;

public:
    Game();
    ~Game() {
        for (const auto player : players) delete player;
    }

    void initializeUnits() const;
    [[nodiscard]] std::vector<Unit*> getAllUnits() const;
    void handleKeyPress(WPARAM wParam);
    void handleMenuInput(WPARAM wParam);
    void handleGameInput(WPARAM wParam);
    void handleGameOverInput(WPARAM wParam);
    void handleHelpInput(WPARAM wParam);
    void draw(HDC hdc);
    void drawMainMenu(HDC hdc);
    void drawGamePlay(HDC hdc);
    void drawGameOver(HDC hdc) const;

    static void drawHelpScreen(HDC hdc);
    void endTurn();
    bool checkGameOver();
    void resetGame();
};

#endif //GAME_H