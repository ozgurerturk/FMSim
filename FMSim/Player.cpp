#include "Player.h"
#include "MatchEngine.h"
#include <algorithm>

Player::Player()
    : _name(""),
    _position(Position::Midfielder),
    _attributes() {
    _stamina = MatchEngine::STARTING_STAMINA;
}

Player::Player(
    std::string name,
    Position position,
    const Attributes& attributes
)
    : _name(name),
    _position(position),
    _attributes(attributes) {
    _stamina = MatchEngine::STARTING_STAMINA;
}

const std::string& Player::getName() const {
    return _name;
}

Position Player::getPosition() const {
    return _position;
}

const Attributes Player::getAttributes() const {
    return _attributes;
}

double Player::getStamina() const {
    return _stamina;
}

void Player::useStamina(double effortCoefficient) {
    double cost = calculateStaminaCost(effortCoefficient);
    _stamina = std::max(0.0, _stamina - cost);
}

double Player::calculateStaminaCost(double effortCoefficient) const {
    double staminaAttr = static_cast<double>(getAttributes().getStamina()); // 0-100

    double alpha = 0.5;
    double calibration = 1.0; // later

    double attributeFactor = 1.0 - alpha * (staminaAttr / 100.0);
    attributeFactor = std::clamp(attributeFactor, 0.5, 1.0);

    return effortCoefficient * attributeFactor * calibration;
}
