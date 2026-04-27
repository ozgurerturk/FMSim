#include "PassEvent.h"

PassEvent::PassEvent(PassType passType)
    : passType(passType) {
}

double PassEvent::getEffortCoefficient() const {
    switch (passType) {
    case PassType::ShortPass:
        return 0.20;
    case PassType::LongPass:
        return 0.25;
    case PassType::ThroughBall:
        return 0.25;
    case PassType::Cross:
        return 0.30;
    case PassType::OneTwoPass:
        return 0.35;
    case PassType::SwitchOfPlay:
        return 0.30;
    case PassType::CornerKick:
        return 0.10;
    case PassType::CornerKickShort:
        return 0.10;
    case PassType::BackPass:
        return 0.15;
    case PassType::CutBack:
        return 0.25;
    case PassType::ThrowIn:
        return 0.05;
    case PassType::ThrowInLong:
        return 0.10;
    case PassType::GoalKick:
        return 0.10;
    case PassType::GoalkeeperThrow:
        return 0.15;
    case PassType::FreeKickPass:
        return 0.10;
    case PassType::FreeKickCross:
        return 0.15;
    case PassType::Clearance:
        return 0.25;
    default:
        return 0.20; // Default effort coefficient for unknown pass types
    }
}

std::string PassEvent::getName() const {
    switch (passType) {
    case PassType::ShortPass:
        return "Short Pass";
    case PassType::LongPass:
        return "Long Pass";
    case PassType::ThroughBall:
        return "Through Ball";
    case PassType::Cross:
        return "Cross";
    case PassType::OneTwoPass:
        return "One-Two Pass";
    case PassType::SwitchOfPlay:
        return "Switch of Play";
    case PassType::CornerKick:
        return "Corner Kick";
    case PassType::CornerKickShort:
        return "Corner Kick Short";
    case PassType::BackPass:
        return "Back Pass";
    case PassType::CutBack:
        return "Cut Back";
    case PassType::ThrowIn:
        return "Throw-In";
    case PassType::ThrowInLong:
        return "Long Throw-In";
    case PassType::GoalKick:
        return "Goal Kick";
    case PassType::GoalkeeperThrow:
        return "Goalkeeper Throw";
    case PassType::FreeKickPass:
        return "Free Kick Pass";
    case PassType::FreeKickCross:
        return "Free Kick Cross";
    case PassType::Clearance:
        return "Clearance";
    default:
        return "Unknown Pass Type";
    }
}

PassType PassEvent::getPassType() const {
    return passType;
}
