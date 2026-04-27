#pragma once
#include <string>
#include "MatchDefinitions.h"
#include "Attributes.h"

class Player {
public:
    Player();
    Player(
        std::string name,
        Position position,
        const Attributes& attributes
    );

    const std::string& getName() const;
    Position getPosition() const;
    const Attributes getAttributes() const;
    void useStamina(double effortCoefficient);
    double stamina;

    double calculateStaminaCost(double effortCoefficient) const;

private:
    std::string _name;
    Position _position;
    Attributes _attributes;
};
