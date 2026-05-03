#include "GoalKeeperEvent.h"

GoalKeeperEvent::GoalKeeperEvent(GoalKeeperEventType eventType)
    : eventType(eventType) {
}

double GoalKeeperEvent::getEffortCoefficient() const {
    switch (eventType) {
        using enum GoalKeeperEventType;
    case GoalkeeperSave:
        return 0.4;
    case GoalkeeperPunch:
        return 0.3;
    case GoalkeeperCatch:
        return 0.5;
    default:
        return 0.4; // Default effort coefficient for unknown goalkeeper events
    }
}

std::string GoalKeeperEvent::getName() const {
    switch (eventType) {
        using enum GoalKeeperEventType;
    case GoalkeeperSave:
        return "Goalkeeper Save";
    case GoalkeeperPunch:
        return "Goalkeeper Punch";
    case GoalkeeperCatch:
        return "Goalkeeper Catch";
    default:
        return "Goalkeeper Event";
    }
}

GoalKeeperEventType GoalKeeperEvent::getEventType() const {
    return eventType;
}
