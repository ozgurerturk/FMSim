#include "ShotEvent.h"

ShotEvent::ShotEvent(ShotType shotType)
    : shotType(shotType) {
}

double ShotEvent::getEffortCoefficient() const {
    switch (shotType) {
    case ShotType::Shoot:
        return 1.0;
    case ShotType::FreeKickShot:
        return 0.4;
    case ShotType::LobbedShot:
        return 1.1;
    case ShotType::PenaltyShot:
        return 0.3;
    case ShotType::TapIn:
        return 0.1;
    case ShotType::Header:
        return 1.2;
    case ShotType::FreeKickHeader:
        return 1.2;
    default:
        return 1.0;
    }
}

std::string ShotEvent::getName() const {
    switch (shotType) {
    case ShotType::Shoot:
        return "Shoot";
    case ShotType::FreeKickShot:
        return "Free Kick Shot";
    case ShotType::LobbedShot:
        return "Lobbed Shot";
    case ShotType::PenaltyShot:
        return "Penalty Shot";
    case ShotType::TapIn:
        return "Tap In";
    case ShotType::Header:
        return "Header";
    case ShotType::FreeKickHeader:
        return "Free Kick Header";
    default:
        return "Unknown Shot Event";
    }
}

ShotType ShotEvent::getShotType() const {
    return shotType;
}
