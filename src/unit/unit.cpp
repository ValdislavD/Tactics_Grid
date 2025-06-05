#include "../../include/unit/unit.h"
#include <windows.h>

Unit::Unit(
    const int h, std::string n, const wchar_t *s, COLORREF col, const int p,
    std::pair<int, int> pos, const int ar, const int d, const int mr, const bool cmd
) : health(h), maxHealth(h), name(std::move(n)), symbol(s), color(col), player(p),
    position(std::move(pos)), attackRange(ar), damage(d), movementRange(mr), canMoveDiagonally(cmd) {}

void Unit::attack(Unit *target) {
    if (target != this) {
        target->takeDamage(damage);
    }
}

void Unit::takeDamage(const int amount) {
    health -= amount;

    if (health < 0) health = 0;
}

bool Unit::isAlive() const {
    return health > 0;
}

int Unit::getHealth() const {
    return health;
}

int Unit::getMaxHealth() const {
    return maxHealth;
}

std::string Unit::getName() const {
    return name;
}

const wchar_t *Unit::getSymbol() const {
    return symbol;
}

COLORREF Unit::getColor() const {
    return color;
}


int Unit::getPlayer() const {
    return player;
}

std::pair<int, int> Unit::getPosition() const {
    return position;
}

int Unit::getAttackRange() const {
    return attackRange;
}

int Unit::getDamage() const {
    return damage;
}

int Unit::getMovementRange() const {
    return movementRange;
}

bool Unit::canMoveDiagonal() const {
    return canMoveDiagonally;
}

void Unit::setPosition(const std::pair<int, int> &pos) {
    position = pos;
}