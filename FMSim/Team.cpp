#include "Team.h"
#include "MatchDefinitions.h"

namespace {
    double safeAverage(double total, int count) {
        return count > 0 ? total / count : 0.0;
    }
}

TeamStrength::TeamStrength()
    : _attack(50),
    _defense(50),
    _goalkeeping(50) {
}

TeamStrength::TeamStrength(double attack, double defense, double goalkeeping)
    : _attack(attack),
    _defense(defense),
    _goalkeeping(goalkeeping) {
}

double TeamStrength::getAttack() const {
    return _attack;
}

double TeamStrength::getDefense() const {
    return _defense;
}

double TeamStrength::getGoalkeeping() const {
    return _goalkeeping;
}

Team::Team()
    : _name(""),
    _players(),
    _teamTactic(TacticType::Possession),
    _defenseTactic(DefenseTacticType::Pressing) {
}

Team::Team(const std::string& name, const std::vector<Player>& Players, TacticType teamTactic, DefenseTacticType defenseTactic)
    : _name(name),
    _players(Players),
    _teamTactic(teamTactic),
    _defenseTactic(defenseTactic) {
    _tactic = Tactic(teamTactic);
    _defenseTacticObj = DefenseTactic(defenseTactic);
}

const std::string& Team::getName() const {
    return _name;
}

const std::vector<Player>& Team::getPlayers() const {
    return _players;
}

TeamStrength Team::getTeamStrength() const {
    double attack = 0;
    double defense = 0;
    double goalkeeping = 0;
    for (const auto& player : _players) {
        switch (player.getPosition()) {
            using enum Position;
        case Attacker:
            attack += player.getAttributes().getAttack();
            break;
        case Defender:
            defense += player.getAttributes().getDefense();
            break;
        case Midfielder:
            attack += player.getAttributes().getAttack();
            defense += player.getAttributes().getDefense();
            break;
        case Goalkeeper:
            goalkeeping += player.getAttributes().getGoalKeeping();
            break;
        }
    }

    return TeamStrength(
        safeAverage(attack, getAttackersCount()),
        safeAverage(defense, getDefendersCount()),
        goalkeeping);
}

int Team::getAttackersCount() const {
    int count = 0;
    for (const auto& player : _players) {
        if (player.getPosition() == Position::Attacker || player.getPosition() == Position::Midfielder) {
            count++;
        }
    }
    return count;
}

int Team::getDefendersCount() const {
    int count = 0;
    for (const auto& player : _players) {
        if (player.getPosition() == Position::Defender || player.getPosition() == Position::Midfielder) {
            count++;
        }
    }
    return count;
}

const Player* Team::getGoalKeeper() const {
    for (const auto& player : _players) {
        if (player.getPosition() == Position::Goalkeeper) {
            return &player;
        }
    }
    return nullptr;
}

const Player* Team::pickAttacker() const {
    // TODO: Implement a more sophisticated attacker selection logic based on attributes and tactics
    return nullptr;
}

TacticType Team::getCurrentTeamTacticName() const {
    return _teamTactic;
}

Tactic& Team::getCurrentTeamTactic() {
    return _tactic;
}

DefenseTacticType Team::getCurrentTeamDefenseTacticName() const {
    return _defenseTactic;
}

DefenseTactic& Team::getCurrentTeamDefenseTactic() {
    return _defenseTacticObj;
}

const DefenseTactic& Team::getCurrentTeamDefenseTactic() const {
    return _defenseTacticObj;
}

const double* Team::getTeamStamina() const {
    return nullptr;
}
