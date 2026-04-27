#include "GoalKeeperEvent.h"

GoalKeeperEvent::GoalKeeperEvent(GoalKeeperEventType eventType)
    : eventType(eventType) {
}

double GoalKeeperEvent::getEffortCoefficient() const {
    switch (eventType) {
    case GoalKeeperEventType::GoalkeeperSave:
        return 0.4;
    case GoalKeeperEventType::GoalkeeperPunch:
        return 0.3;
    case GoalKeeperEventType::GoalkeeperCatch:
        return 0.5;
    default:
        return 0.4; // Default effort coefficient for unknown goalkeeper events
    }
}

std::string GoalKeeperEvent::getName() const {
    switch (eventType) {
    case GoalKeeperEventType::GoalkeeperSave:
        return "Goalkeeper Save";
    case GoalKeeperEventType::GoalkeeperPunch:
        return "Goalkeeper Punch";
    case GoalKeeperEventType::GoalkeeperCatch:
        return "Goalkeeper Catch";
    default:
        return "Goalkeeper Event";
    }
}

GoalKeeperEventType GoalKeeperEvent::getEventType() const {
    return eventType;
}
