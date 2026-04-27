#pragma once
#include "Event.h"

enum class PassType {
    ShortPass,
    LongPass,
    ThroughBall,
    Cross,
    OneTwoPass,
    SwitchOfPlay,
    CornerKick,
    CornerKickShort,
    BackPass,
    CutBack,
    ThrowIn,
    ThrowInLong,
    GoalKick,
    GoalkeeperThrow,
    FreeKickPass,
    FreeKickCross,
    Clearance
};

class PassEvent : public Event {
public:
    explicit PassEvent(PassType passType);

    double getEffortCoefficient() const override;

    std::string getName() const override;

    PassType getPassType() const;
private:
    PassType passType;
};
