#ifndef MAGE_H
#define MAGE_H

#include <utility>

#include "unit.h"
#include "../constants.h"

// Колдун
class Mage final : public Unit {
public:
    Mage(int p, const std::pair<int, int> &pos);
};

#endif //MAGE_H