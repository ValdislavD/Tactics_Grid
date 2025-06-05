#include "../../include/unit/archer.h"
#include "../../include/constants.h"

Archer::Archer(const int p, const std::pair<int, int> &pos) : Unit(
    30, "Archer", ARCHER_SYMBOL, p == 1 ? COLOR_BLUE : COLOR_RED, p, pos, 3, 10, 2, true
) {}