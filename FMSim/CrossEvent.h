#pragma once
#include "Event.h"

enum class CrossType {
    Cross,
    FreeKickCross,
    CornerKick
};

class CrossEvent : public Event {
public:
    explicit CrossEvent(CrossType crossType);
    double getEffortCoefficient() const override;
    std::string getName() const override;
    CrossType getCrossType() const;

private:
    CrossType crossType;
};
