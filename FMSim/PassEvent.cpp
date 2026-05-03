#include "PassEvent.h"

PassEvent::PassEvent(PassType passType)
    : passType(passType) {
}

double PassEvent::getEffortCoefficient() const {
    switch (passType) {
        using enum PassType;
    case ShortPass:
        return 0.20;
    case LongPass:
        return 0.25;
    case ThroughBall:
        return 0.25;
    case Cross:
        return 0.30;
    case OneTwoPass:
        return 0.35;
    case SwitchOfPlay:
        return 0.30;
    case CornerKick:
        return 0.10;
    case CornerKickShort:
        return 0.10;
    case BackPass:
        return 0.15;
    case CutBack:
        return 0.25;
    case ThrowIn:
        return 0.05;
    case ThrowInLong:
        return 0.10;
    case GoalKick:
        return 0.10;
    case GoalkeeperThrow:
        return 0.15;
    case FreeKickPass:
        return 0.10;
    case FreeKickCross:
        return 0.15;
    case Clearance:
        return 0.25;
    default:
        return 0.20; // Default effort coefficient for unknown pass types
    }
}

std::string PassEvent::getName() const {
    switch (passType) {
        using enum PassType;
    case ShortPass:
        return "Short Pass";
    case LongPass:
        return "Long Pass";
    case ThroughBall:
        return "Through Ball";
    case Cross:
        return "Cross";
    case OneTwoPass:
        return "One-Two Pass";
    case SwitchOfPlay:
        return "Switch of Play";
    case CornerKick:
        return "Corner Kick";
    case CornerKickShort:
        return "Corner Kick Short";
    case BackPass:
        return "Back Pass";
    case CutBack:
        return "Cut Back";
    case ThrowIn:
        return "Throw-In";
    case ThrowInLong:
        return "Long Throw-In";
    case GoalKick:
        return "Goal Kick";
    case GoalkeeperThrow:
        return "Goalkeeper Throw";
    case FreeKickPass:
        return "Free Kick Pass";
    case FreeKickCross:
        return "Free Kick Cross";
    case Clearance:
        return "Clearance";
    default:
        return "Unknown Pass Type";
    }
}

PassType PassEvent::getPassType() const {
    return passType;
}
