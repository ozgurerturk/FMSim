#pragma once
#include "Event.h"

enum class GoalKeeperEventType {
    GoalkeeperSave,
    GoalkeeperPunch,
    GoalkeeperCatch
};

class GoalKeeperEvent : public Event {
public:

    explicit GoalKeeperEvent(GoalKeeperEventType eventType);
    double getEffortCoefficient() const override;
    std::string getName() const override;
    GoalKeeperEventType getEventType() const;

private:
    GoalKeeperEventType eventType;
};
