#pragma once
#include "MatchDefinitions.h"
#include <vector>
#include "MatchEvents.h"

struct ZoneStep
{
    Zone from;
    Zone to;
    bool isLongBall = false;
    bool isRotation = false;
};

struct ZonePath
{
    std::vector<ZoneStep> steps;
};

class Tactic
{
public:
    Tactic();
    Tactic(TacticType type);

    TacticType tacticType;
    std::vector<ZonePath> paths;
    ZonePath chosenPath;

    ZonePath GetRandomPath(Zone ballZone) const;
    AttackEvent GetAttackEvent(Zone ballZone) const;
    Zone GetNextZone(Zone ballZone) const;
    AttackEvent StartAttack(Zone ballZone);
    void ResetAttack();

private:
    TacticType _tacticType;
    bool newAttackPathChosen = false;
};

class DefenseTactic
{
public:
    DefenseTactic();
    DefenseTactic(DefenseTacticType type);

    DefenseTacticType defenseTacticType;
    DefenseEvent RespondToAttack(AttackEvent attackEvent);

private:
    DefenseTacticType _defenseTacticType;
};
