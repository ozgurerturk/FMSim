#pragma once
#include "MatchDefinitions.h"
#include "Tactics.h"

class TacticFactory {
public:
    static Tactic CreateWingPlay();
    static Tactic CreateLongBall();
    static Tactic CreateTikiTaka();
    static Tactic CreateCounterAttack();
    static Tactic CreatePossessionGame();

    static DefenseTactic CreatePressing();
    static DefenseTactic CreateManMarking();
    static DefenseTactic CreateZonalMarking();
    static DefenseTactic CreateCounterPressing();
};
