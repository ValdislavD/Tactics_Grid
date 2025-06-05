#ifndef WARRIOR_H
#define WARRIOR_H

#include "unit.h"

// Воин
class Warrior final : public Unit {
public:
    Warrior(int p, const std::pair<int, int> &pos);
};

#endif //WARRIOR_H