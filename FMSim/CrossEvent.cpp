#include "CrossEvent.h"

CrossEvent::CrossEvent(CrossType crossType)
    : crossType(crossType) {
}

double CrossEvent::getEffortCoefficient() const {
    switch (crossType) {
        using enum CrossType;
    case Cross:
        return 0.3;
    case FreeKickCross:
        return 0.15;
    case CornerKick:
        return 0.1;
    default:
        return 0.3;
    }
}

std::string CrossEvent::getName() const {
    switch (crossType) {
        using enum CrossType;
    case Cross:
        return "Cross";
    case FreeKickCross:
        return "Free Kick Cross";
    case CornerKick:
        return "Corner Kick";
    default:
        return "Cross";
    }
}

CrossType CrossEvent::getCrossType() const {
    return crossType;
}
