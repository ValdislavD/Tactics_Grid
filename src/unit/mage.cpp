#include "../../include/unit/mage.h"
#include "../../include/constants.h"

Mage::Mage(const int p, const std::pair<int, int> &pos) : Unit(
    25, "Mage", MAGE_SYMBOL, p == 1 ? COLOR_BLUE : COLOR_RED, p, pos, 4, 15, 1, false
) {}