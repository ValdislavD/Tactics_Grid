#include "../../include/factory/unit_factory.h"
#include "../../include/unit/warrior.h"
#include "../../include/unit/archer.h"
#include "../../include/unit/mage.h"

Unit* UnitFactory::createUnit(const std::string& type, const int player, const std::pair<int, int> &position) {
    if (type == "Warrior") {
        return new Warrior(player, position);
    }

    if (type == "Archer") {
        return new Archer(player, position);
    }

    if (type == "Mage") {
        return new Mage(player, position);
    }

    return nullptr;
}