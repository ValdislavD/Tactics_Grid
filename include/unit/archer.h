#ifndef ARCHER_H
#define ARCHER_H

#include "unit.h"

// Лучник
class Archer final : public Unit {
public:
    Archer(int p, const std::pair<int, int> &pos);
};

#endif //ARCHER_H