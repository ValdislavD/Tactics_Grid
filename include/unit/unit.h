#ifndef UNIT_H
#define UNIT_H

#include <string>
#include <windows.h>

class Unit {
protected:
    int health;
    int maxHealth;
    std::string name;
    const wchar_t* symbol;
    COLORREF color{};
    int player;
    std::pair<int, int> position;
    int attackRange;
    int damage;
    int movementRange;
    bool canMoveDiagonally;

public:
    Unit(
        int h, std::string n, const wchar_t* s, COLORREF col, int p,
        std::pair<int, int> pos, int ar, int d, int mr, bool cmd
    );
    virtual ~Unit() = default;

    virtual void attack(Unit* target);
    void takeDamage(int amount);
    [[nodiscard]] bool isAlive() const;
    [[nodiscard]] int getHealth() const;
    [[nodiscard]] int getMaxHealth() const;
    [[nodiscard]] std::string getName() const;
    [[nodiscard]] const wchar_t* getSymbol() const;
    [[nodiscard]] COLORREF getColor() const;
    [[nodiscard]] int getPlayer() const;
    [[nodiscard]] std::pair<int, int> getPosition() const;
    [[nodiscard]] int getAttackRange() const;
    [[nodiscard]] int getDamage() const;
    [[nodiscard]] int getMovementRange() const;
    [[nodiscard]] bool canMoveDiagonal() const;
    void setPosition(const std::pair<int, int> &pos);
};

#endif //UNIT_H