#include "../../include/game/game.h"

Game::Game() : currentState(MAIN_MENU), currentPlayerIndex(0), selectedUnit(nullptr),
               isSelectingTarget(false), isMovingUnit(false), selectedMenuItem(0) {
    players.push_back(new HumanPlayer(1));
    players.push_back(new HumanPlayer(2));
    initializeUnits();
}

void Game::initializeUnits() const {
    // Игрок 1 (синие юниты)
    players[0]->addUnit(UnitFactory::createUnit("Warrior", 1, {1, 1}));
    players[0]->addUnit(UnitFactory::createUnit("Archer", 1, {2, 1}));
    players[0]->addUnit(UnitFactory::createUnit("Mage", 1, {3, 1}));

    // Игрок 2 (красные юниты)
    players[1]->addUnit(UnitFactory::createUnit("Warrior", 2, {8, 8}));
    players[1]->addUnit(UnitFactory::createUnit("Archer", 2, {7, 8}));
    players[1]->addUnit(UnitFactory::createUnit("Mage", 2, {6, 8}));
}

std::vector<Unit *> Game::getAllUnits() const {
    std::vector<Unit *> allUnits;

    for (const auto player: players) {
        for (auto unit: player->getUnits()) {
            allUnits.push_back(unit);
        }
    }

    return allUnits;
}

void Game::handleKeyPress(WPARAM wParam) {
    if (currentState == MAIN_MENU) {
        handleMenuInput(wParam);
    } else if (currentState == GAME_PLAY) {
        handleGameInput(wParam);
    } else if (currentState == GAME_OVER) {
        handleGameOverInput(wParam);
    } else if (currentState == HELP_SCREEN) {
        handleHelpInput(wParam);
    }
}

void Game::handleMenuInput(WPARAM wParam) {
    switch (wParam) {
        case 'W':
        case VK_UP:
            selectedMenuItem = (selectedMenuItem - 1 + 3) % 3;
            break;
        case 'S':
        case VK_DOWN:
            selectedMenuItem = (selectedMenuItem + 1) % 3;
            break;
        case VK_RETURN:
            if (selectedMenuItem == 0) {
                // Новая игра
                currentState = GAME_PLAY;
                static_cast<HumanPlayer *>(players[0])->setStatus("Player 1's turn. Select unit with Enter.");
            } else if (selectedMenuItem == 1) {
                // Помощь
                currentState = HELP_SCREEN;
            } else if (selectedMenuItem == 2) {
                // Выход
                PostQuitMessage(0);
            }
            break;
        default: ;
    }

    InvalidateRect(Constants::hwnd, NULL, TRUE);
}

void Game::handleGameInput(WPARAM wParam) {
    auto *currentPlayer = dynamic_cast<HumanPlayer *>(players[currentPlayerIndex]);
    const auto enemy = dynamic_cast<HumanPlayer *>(players[(currentPlayerIndex + 1) % 2]);

    if (wParam == 'W' || wParam == 'A' || wParam == 'S' || wParam == 'D' ||
        wParam == 'Q' || wParam == 'E' || wParam == 'Z' || wParam == 'C') {
        board.moveCursor(wParam);
    } else {
        switch (wParam) {
            case VK_RETURN: // Enter
                if (!selectedUnit) {
                    // Выбор юнита
                    const auto cursorPos = board.getCursorPos();
                    for (const auto unit: currentPlayer->getUnits()) {
                        if (unit->isAlive() && unit->getPosition() == cursorPos) {
                            selectedUnit = unit;
                            currentPlayer->setStatus(
                                "Selected " + unit->getName() + ". Press M to move or F to attack.");
                            break;
                        }
                    }
                } else if (isMovingUnit) {
                    // Подтверждение перемещения
                    if (const auto targetPos = board.getCursorPos(); board.isCellReachable(
                        targetPos.first, targetPos.second)) {
                        selectedUnit->setPosition(targetPos);
                        board.clearReachableCells();
                        isMovingUnit = false;
                        currentPlayer->setStatus(selectedUnit->getName() + " moved. Turn ended.");

                        // Завершаем ход после перемещения
                        endTurn();
                    }
                } else if (isSelectingTarget) {
                    // Подтверждение атаки
                    const auto targetPos = board.getCursorPos();
                    const int distance = std::abs(selectedUnit->getPosition().first - targetPos.first) +
                                         std::abs(selectedUnit->getPosition().second - targetPos.second);

                    if (distance <= selectedUnit->getAttackRange()) {
                        for (const auto enemyUnit: enemy->getUnits()) {
                            if (enemyUnit->isAlive() && enemyUnit->getPosition() == targetPos) {
                                selectedUnit->attack(enemyUnit);
                                board.clearAttackableCells();
                                isSelectingTarget = false;

                                // Удаляем мертвых юнитов
                                enemy->removeDeadUnits();

                                currentPlayer->setStatus(selectedUnit->getName() + " attacked " + enemyUnit->getName());

                                // Проверяем конец игры
                                if (checkGameOver()) return;

                                // Завершаем ход после атаки
                                endTurn();
                                return;
                            }
                        }
                    }
                }
                break;

            case 'M': // M - перемещение
                if (selectedUnit && !isSelectingTarget && !isMovingUnit) {
                    isMovingUnit = true;
                    board.calculateReachableCells(selectedUnit, getAllUnits());
                    currentPlayer->setStatus(
                        "Moving " + selectedUnit->getName() + ". Select destination and press Enter."
                    );
                }
                break;

            case 'F': // F - атака
                if (selectedUnit && !isMovingUnit && !isSelectingTarget) {
                    isSelectingTarget = true;
                    board.calculateAttackableCells(selectedUnit, getAllUnits());
                    currentPlayer->setStatus(
                        "Attacking with " + selectedUnit->getName() + ". Select target and press Enter."
                    );
                }
                break;

            case VK_ESCAPE: // Escape
                if (isMovingUnit || isSelectingTarget) {
                    board.clearReachableCells();
                    board.clearAttackableCells();
                    isSelectingTarget = false;
                    isMovingUnit = false;
                    currentPlayer->setStatus(
                        "Selected " + selectedUnit->getName() + ". Press M to move or F to attack."
                    );
                } else if (selectedUnit) {
                    // Отмена выбора юнита
                    selectedUnit = nullptr;
                    currentPlayer->setStatus("Select unit with Enter.");
                } else {
                    // Возврат в меню
                    currentState = MAIN_MENU;
                }
                break;
            default: ;
        }
    }
    InvalidateRect(Constants::hwnd, NULL, TRUE);
}

void Game::handleGameOverInput(WPARAM wParam) {
    switch (wParam) {
        case 'Y':
        case VK_RETURN:
            resetGame();
            currentState = GAME_PLAY;
            break;
        case 'N':
        case VK_ESCAPE:
            currentState = MAIN_MENU;
            break;
        default: ;
    }
    InvalidateRect(Constants::hwnd, nullptr, TRUE);
}

void Game::handleHelpInput(WPARAM wParam) {
    // Любая клавиша возвращает в меню
    currentState = MAIN_MENU;
    InvalidateRect(Constants::hwnd, nullptr, TRUE);
}

void Game::draw(HDC hdc) {
    Constants::hBrush = CreateSolidBrush(COLOR_WHITE);
    FillRect(hdc, &Constants::clientRect, Constants::hBrush);
    DeleteObject(Constants::hBrush);

    if (currentState == MAIN_MENU) {
        drawMainMenu(hdc);
    } else if (currentState == GAME_PLAY) {
        drawGamePlay(hdc);
    } else if (currentState == GAME_OVER) {
        drawGameOver(hdc);
    } else if (currentState == HELP_SCREEN) {
        drawHelpScreen(hdc);
    }
}

void Game::drawMainMenu(HDC hdc) {
    // Заголовок
    HFONT hBigFont = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH,
                                TEXT("Arial"));
    auto hOldFont = static_cast<HFONT>(SelectObject(hdc, hBigFont));

    SetTextColor(hdc, COLOR_BLUE);
    TextOutW(hdc, WINDOW_WIDTH / 2 - 175, 100, L"Стратегическая игра", 19);

    SelectObject(hdc, hOldFont);
    DeleteObject(hBigFont);

    // Меню
    HFONT hMenuFont = CreateFont(36, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                 OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH,
                                 TEXT("Arial"));
    hOldFont = (HFONT) SelectObject(hdc, hMenuFont);

    for (int i = 0; i < 3; i++) {
        const wchar_t *menuItems[] = {L"Новая игра", L"Помощь", L"Выход"};
        if (i == selectedMenuItem) {
            SetTextColor(hdc, COLOR_RED);
        } else {
            SetTextColor(hdc, COLOR_BLACK);
        }
        TextOutW(hdc, WINDOW_WIDTH / 2 - 100, 200 + i * 50, menuItems[i], wcslen(menuItems[i]));
    }

    SelectObject(hdc, hOldFont);
    DeleteObject(hMenuFont);

    // Подсказка
    const HFONT hSmallFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                  OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH,
                                  TEXT("Arial"));
    hOldFont = static_cast<HFONT>(SelectObject(hdc, hSmallFont));

    SetTextColor(hdc, COLOR_GRAY);
    TextOutW(hdc, WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT - 50, L"Используйте стрелки и Enter для выбора", 37);

    SelectObject(hdc, hOldFont);
    DeleteObject(hSmallFont);
}


void Game::drawGamePlay(HDC hdc) {
    board.draw(hdc, getAllUnits());

    dynamic_cast<HumanPlayer *>(players[currentPlayerIndex])->drawStatus(hdc);

    // Отображаем текущего игрока
    std::wstring turnText;
    COLORREF textColor;
    if (players[currentPlayerIndex]->getUnits()[0]->getPlayer() == 1) {
        turnText = L"Ход Игрока 1 (Синие)";
        textColor = COLOR_BLUE;
    } else {
        turnText = L"Ход Игрока 2 (Красные)";
        textColor = COLOR_RED;
    }

    const HFONT hFont = CreateFont(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
    const auto hOldFont = static_cast<HFONT>(SelectObject(hdc, hFont));

    SetTextColor(hdc, textColor);
    TextOutW(hdc, WINDOW_WIDTH - 250, 20, turnText.c_str(), (int) turnText.length());

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

void Game::drawGameOver(const HDC hdc) const {
    const bool player1Alive = players[0]->hasAliveUnits();
    const bool player2Alive = players[1]->hasAliveUnits();

    std::wstring message;
    if (player1Alive) {
        message = L"Победил Игрок 1 (Синие)!";
    } else if (player2Alive) {
        message = L"Победил Игрок 2 (Красные)!";
    } else {
        message = L"Ничья!";
    }

    const HFONT hBigFont = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH,
                                TEXT("Arial"));
    auto hOldFont = static_cast<HFONT>(SelectObject(hdc, hBigFont));

    SetTextColor(hdc, player1Alive ? COLOR_BLUE : (player2Alive ? COLOR_RED : COLOR_PURPLE));
    TextOutW(hdc, WINDOW_WIDTH / 2 - 200, 150, message.c_str(), static_cast<int>(message.length()));

    SelectObject(hdc, hOldFont);
    DeleteObject(hBigFont);

    HFONT hMenuFont = CreateFont(36, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                 OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH,
                                 TEXT("Arial"));
    hOldFont = (HFONT) SelectObject(hdc, hMenuFont);

    SetTextColor(hdc, COLOR_BLACK);
    TextOutW(hdc, WINDOW_WIDTH / 2 - 100, 250, L"Новая игра (Enter)", 18);
    TextOutW(hdc, WINDOW_WIDTH / 2 - 70, 300, L"Меню (Esc)", 10);

    SelectObject(hdc, hOldFont);
    DeleteObject(hMenuFont);
}

void Game::drawHelpScreen(const HDC hdc) {
    // Заголовок
    HFONT hBigFont = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH,
                                TEXT("Arial"));
    auto hOldFont = static_cast<HFONT>(SelectObject(hdc, hBigFont));

    SetTextColor(hdc, COLOR_BLUE);
    TextOutW(hdc, WINDOW_WIDTH / 2 - 100, 50, L"Помощь", 6);

    SelectObject(hdc, hOldFont);
    DeleteObject(hBigFont);

    // Текст помощи
    HFONT hHelpFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                 OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH,
                                 TEXT("Arial"));
    hOldFont = static_cast<HFONT>(SelectObject(hdc, hHelpFont));

    SetTextColor(hdc, COLOR_BLACK);

    // Разделы помощи
    const wchar_t *helpSections[] = {
        L"Цель игры:",
        L"Уничтожить все юниты противника.",
        L"",
        L"Управление:",
        L"WASD - перемещение курсора",
        L"QEZC - диагональное перемещение",
        L"Enter - выбор/подтверждение действия",
        L"M - режим перемещения юнита",
        L"F - режим атаки",
        L"Esc - отмена/конец хода",
        L"",
        L"Юниты:",
        L"Воин (🤺) - ближний бой, высокая защита",
        L"Лучник (🏹) - дальний бой, средняя защита",
        L"Маг (🧙) - дальний бой, слабая защита",
        L"",
        L"Нажмите любую клавишу для возврата в меню"
    };

    int yPos = 120;
    for (int i = 0; i < std::size(helpSections); i++) {
        if (i == 0 || i == 3 || i == 11) {
            // Заголовки разделов
            SetTextColor(hdc, COLOR_RED);
            TextOutW(hdc, 50, yPos, helpSections[i], wcslen(helpSections[i]));
            yPos += 30;
        } else {
            // Обычный текст
            SetTextColor(hdc, COLOR_BLACK);
            TextOutW(hdc, 50, yPos, helpSections[i], wcslen(helpSections[i]));
            yPos += 25;
        }
    }

    SelectObject(hdc, hOldFont);
    DeleteObject(hHelpFont);
}

void Game::endTurn() {
    board.clearReachableCells();
    board.clearAttackableCells();
    selectedUnit = nullptr;
    isSelectingTarget = false;
    isMovingUnit = false;

    currentPlayerIndex = (currentPlayerIndex + 1) % 2;

    std::stringstream ss;

    ss << "Player " << (currentPlayerIndex + 1) << "'s turn. Select unit with Enter.";

    dynamic_cast<HumanPlayer*>(players[currentPlayerIndex])->setStatus(ss.str());
}

bool Game::checkGameOver() {
    const bool player1Alive = players[0]->hasAliveUnits();
    const bool player2Alive = players[1]->hasAliveUnits();

    if (!player1Alive || !player2Alive) {
        currentState = GAME_OVER;
        InvalidateRect(Constants::hwnd, nullptr, TRUE);
        return true;
    }

    return false;
}

void Game::resetGame() {
    // Очищаем текущее состояние игры
    for (const auto player : players) delete player;

    players.clear();

    // Создаем новых игроков
    players.push_back(new HumanPlayer(1));
    players.push_back(new HumanPlayer(2));

    // Инициализируем юнитов
    initializeUnits();

    // Сбрасываем состояние игры
    currentPlayerIndex = 0;
    selectedUnit = nullptr;
    isSelectingTarget = false;
    isMovingUnit = false;
    board = GameBoard(); // Сбрасываем доску

    // Устанавливаем начальное сообщение
    dynamic_cast<HumanPlayer*>(players[0])->setStatus("Player 1's turn. Select unit with Enter.");
}
