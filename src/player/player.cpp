#include "../../include/player/player.h"

#include <algorithm>

void Player::addUnit(Unit *unit) {
    units.push_back(unit);
}

const std::vector<Unit *> &Player::getUnits() const {
    return units;
}

bool Player::hasAliveUnits() const {
    for (const auto& unit : units) {
        if (unit->isAlive()) return true;
    }

    return false;
}

void Player::removeDeadUnits() {
    units.erase(std::remove_if(units.begin(), units.end(), [](const Unit* unit) {
        if (!unit->isAlive()) {
            delete unit;
            return true;
        }
        return false;
    }), units.end());
}