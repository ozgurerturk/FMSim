#include "DribbleEvent.h"

DribbleEvent::DribbleEvent(DribbleType dribbleType)
    : dribbleType(dribbleType) {
}

double DribbleEvent::getEffortCoefficient() const {
    switch (dribbleType) {
    case DribbleType::Dribble:
        return 1.40;
    case DribbleType::DribbleCutInside:
        return 1.50;
    default:
        return 1.40; // Default effort coefficient for unknown dribble types
    }
}

std::string DribbleEvent::getName() const {
    switch (dribbleType) {
    case DribbleType::Dribble:
        return "Dribble";
    case DribbleType::DribbleCutInside:
        return "Dribble Cut Inside";
    default:
        return "Dribble"; // Default name for unknown dribble types
    }
}

DribbleType DribbleEvent::getDribbleType() const {
    return dribbleType;
}
