#pragma once
#include <string>
#include <vector>
#include "Player.h"
#include "MatchDefinitions.h"
#include "Tactics.h"

class TeamStrength {
public:
    TeamStrength();
    TeamStrength(double attack, double defense, double goalkeeping);

    double getAttack() const;
    double getDefense() const;
    double getGoalkeeping() const;

private:
    double _attack;
    double _defense;
    double _goalkeeping;
};

class Team {
public:
    Team();
    Team(const std::string& name, const std::vector<Player>& Players, TacticType teamTactic, DefenseTacticType defenseTactic);

    const std::string& getName() const;
    const std::vector<Player>& getPlayers() const;

    TeamStrength getTeamStrength() const;
    const Player* getGoalKeeper() const;
    const Player* pickAttacker() const;
    int getAttackersCount() const;
    int getDefendersCount() const;
    TacticType getCurrentTeamTacticName() const;
    Tactic& getCurrentTeamTactic();
    DefenseTacticType getCurrentTeamDefenseTacticName() const;
    DefenseTactic& getCurrentTeamDefenseTactic();
    const DefenseTactic& getCurrentTeamDefenseTactic() const;

    const double* getTeamStamina() const;

private:
    std::string _name;
    std::vector<Player> _players;
    TacticType _teamTactic;
    Tactic _tactic;
    DefenseTacticType _defenseTactic;
    DefenseTactic _defenseTacticObj;
};
