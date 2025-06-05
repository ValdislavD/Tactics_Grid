#ifndef UNIT_FACTORY_H
#define UNIT_FACTORY_H

#include "../unit/unit.h"

#include <string>

class UnitFactory {
public:
    static Unit* createUnit(const std::string& type, int player, const std::pair<int, int> &position);
};

#endif //UNIT_FACTORY_H