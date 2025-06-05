#include "../../include/unit/warrior.h"
#include "../../include/constants.h"

Warrior::Warrior(const int p, const std::pair<int, int> &pos) : Unit(
    50, "Warrior", WARRIOR_SYMBOL, p == 1 ? COLOR_BLUE : COLOR_RED, p, pos, 1, 20, 3, true
) {}