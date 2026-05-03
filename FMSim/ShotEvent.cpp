#include "ShotEvent.h"

ShotEvent::ShotEvent(ShotType shotType)
    : shotType(shotType) {
}

double ShotEvent::getEffortCoefficient() const {
    switch (shotType) {
        using enum ShotType;
    case Shoot:
        return 1.0;
    case FreeKickShot:
        return 0.4;
    case LobbedShot:
        return 1.1;
    case PenaltyShot:
        return 0.3;
    case TapIn:
        return 0.1;
    case Header:
        return 1.2;
    case FreeKickHeader:
        return 1.2;
    default:
        return 1.0;
    }
}

std::string ShotEvent::getName() const {
    switch (shotType) {
        using enum ShotType;
    case Shoot:
        return "Shoot";
    case FreeKickShot:
        return "Free Kick Shot";
    case LobbedShot:
        return "Lobbed Shot";
    case PenaltyShot:
        return "Penalty Shot";
    case TapIn:
        return "Tap In";
    case Header:
        return "Header";
    case FreeKickHeader:
        return "Free Kick Header";
    default:
        return "Unknown Shot Event";
    }
}

ShotType ShotEvent::getShotType() const {
    return shotType;
}
