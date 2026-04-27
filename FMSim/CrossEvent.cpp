#include "CrossEvent.h"

CrossEvent::CrossEvent(CrossType crossType)
    : crossType(crossType) {
}

double CrossEvent::getEffortCoefficient() const {
    switch (crossType) {
    case CrossType::Cross:
        return 0.3;
    case CrossType::FreeKickCross:
        return 0.15;
    case CrossType::CornerKick:
        return 0.1;
    default:
        return 0.3;
    }
}

std::string CrossEvent::getName() const {
    switch (crossType) {
    case CrossType::Cross:
        return "Cross";
    case CrossType::FreeKickCross:
        return "Free Kick Cross";
    case CrossType::CornerKick:
        return "Corner Kick";
    default:
        return "Cross";
    }
}

CrossType CrossEvent::getCrossType() const {
    return crossType;
}
