#pragma once
#include "MatchDefinitions.h"
#include <vector>
#include "MatchEvents.h"

struct ZoneStep {
    Zone from;
    Zone to;
    bool isLongBall = false;
    bool isRotation = false;
};

struct ZonePath {
    std::vector<ZoneStep> steps;
};

class Tactic {
public:
    Tactic();
    explicit Tactic(TacticType type);

    TacticType getTacticType() const;
    void setTacticType(TacticType type);
    void setPaths(std::vector<ZonePath> paths);
    void addPath(const ZonePath& path);
    bool hasPaths() const;
    void addStepToLastPath(const ZoneStep& step);
    ZonePath GetRandomPath(Zone ballZone) const;
    AttackEvent GetAttackEvent(Zone ballZone) const;
    Zone GetNextZone(Zone ballZone) const;
    AttackEvent StartAttack(Zone ballZone);
    void ResetAttack();

private:
    TacticType _tacticType;
    std::vector<ZonePath> _paths;
    ZonePath _chosenPath;
    bool _newAttackPathChosen = false;
};

class DefenseTactic {
public:
    DefenseTactic();
    explicit DefenseTactic(DefenseTacticType type);

    DefenseTacticType getDefenseTacticType() const;
    void setDefenseTacticType(DefenseTacticType type);
    DefenseEvent RespondToAttack(AttackEvent attackEvent);

private:
    DefenseTacticType _defenseTacticType;
};
