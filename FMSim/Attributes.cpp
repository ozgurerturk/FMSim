#include "Attributes.h"
#include <math.h>

Attributes::Attributes()
    : _defense(0),
    _attack(0),
    _pace(0),
    _stamina(0),
    _passing(0),
    _tackle(0),
    _dribbling(0),
    _shooting(0),
    _physicality(0),
    _jumping(0),
    _reactionTime(0) {
}

Attributes::Attributes(int defense, int attack, int pace
    , int stamina, int passing, int tackle
    , int dribbling, int shooting, int physicality
    , int jumping, int reactionTime)
    : _defense(defense),
    _attack(attack),
    _pace(pace),
    _stamina(stamina),
    _passing(passing),
    _tackle(tackle),
    _dribbling(dribbling),
    _shooting(shooting),
    _physicality(physicality),
    _jumping(jumping),
    _reactionTime(reactionTime) {
}

int Attributes::getDefense() const {
    return _defense;
}

int Attributes::getAttack() const {
    return _attack;
}

int Attributes::getPace() const {
    return _pace;
}

int Attributes::getStamina() const {
    return _stamina;
}

int Attributes::getPassing() const {
    return _passing;
}

int Attributes::getTackle() const {
    return _tackle;
}

int Attributes::getDribbling() const {
    return _dribbling;
}

int Attributes::getShooting() const {
    return _shooting;
}

int Attributes::getPhysicality() const {
    return _physicality;
}

int Attributes::getJumping() const {
    return _jumping;
}

int Attributes::getReactionTime() const {
    return _reactionTime;
}

int Attributes::getGoalKeeping() const {
    return static_cast<int>(round((_jumping + _reactionTime) / 2.0));
}
