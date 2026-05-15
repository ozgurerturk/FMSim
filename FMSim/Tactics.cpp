#include "Tactics.h"
#include "TacticFactory.h"
#include "PathFinder.h"
#include "ZoneGraph.h"
#include <ranges>
#include <algorithm>
#include <random>
#include <utility>

namespace {
    std::mt19937& randomGenerator() {
        static thread_local std::mt19937 generator{ std::random_device{}() };
        return generator;
    }
}

Tactic::Tactic()
    : _tacticType(TacticType::Possession) {
}

Tactic::Tactic(TacticType type)
    : _tacticType(type) {
    switch (type) {
        using enum TacticType;
    case WingPlay:
        *this = TacticFactory::CreateWingPlay();
        break;
    case LongBall:
        *this = TacticFactory::CreateLongBall();
        break;
    case TikiTaka:
        *this = TacticFactory::CreateTikiTaka();
        break;
    case CounterAttack:
        *this = TacticFactory::CreateCounterAttack();
        break;
    case Possession:
    default:
        *this = TacticFactory::CreatePossessionGame();
        break;
    }
}

TacticType Tactic::getTacticType() const {
    return _tacticType;
}

void Tactic::setTacticType(TacticType type) {
    _tacticType = type;
}

void Tactic::setPaths(std::vector<ZonePath> paths) {
    _paths = std::move(paths);
}

void Tactic::addPath(const ZonePath& path) {
    _paths.push_back(path);
}

bool Tactic::hasPaths() const {
    return !_paths.empty();
}

void Tactic::addStepToLastPath(const ZoneStep& step) {
    if (!_paths.empty()) {
        _paths.back().steps.push_back(step);
    }
}

ZonePath Tactic::GetRandomPath(Zone ballZone) const {
    if (_paths.empty())
        return ZonePath();

    std::vector<ZonePath> candidates;
    for (const auto& path : _paths) {
        auto st = std::find_if(path.steps.begin(), path.steps.end(), [&](const ZoneStep& step) {
            return step.from == ballZone;
            });

        if (st == path.steps.end()) {
            continue;
        }

        ZonePath candidate;
        candidate.steps.assign(st, path.steps.end());
        candidates.push_back(candidate);
    }

    if (candidates.empty()) {
        if (ballZone == Zone::A2) {
            ZonePath shootPath;
            shootPath.steps.push_back(ZoneStep{ Zone::A2, Zone::A2 });
            return shootPath;
        }

        ZoneGraph graph;
        auto fallbackZones = PathFinder::FindPath(graph, ballZone, Zone::A2);
        ZonePath fallbackPath;

        if (fallbackZones.size() >= 2) {
            for (size_t i = 0; i + 1 < fallbackZones.size(); ++i) {
                fallbackPath.steps.push_back(ZoneStep{ fallbackZones[i], fallbackZones[i + 1] });
            }

            fallbackPath.steps.push_back(ZoneStep{ Zone::A2, Zone::A2 });
        }

        return fallbackPath;
    }

    std::uniform_int_distribution<std::size_t> distribution(0, candidates.size() - 1);
    return candidates[distribution(randomGenerator())];
}

AttackEvent Tactic::GetAttackEvent(Zone ballZone) const {
    if (_chosenPath.steps.empty()) {
        return AttackEvent::Clearance;
    }

    auto findZoneStep = [&](const ZoneStep& step) {
        return step.from == ballZone;
        };

    auto st = std::find_if(_chosenPath.steps.begin(), _chosenPath.steps.end(), findZoneStep);

    if (st == _chosenPath.steps.end()) {
        return AttackEvent::Clearance;
    }

    if (st->from == Zone::A2 && st->to == Zone::A2) {
        // TODO - More complex logic will be implemented here later to determine whether to
        // shoot, dribble or back pass based on the situation, player attributes and other factors.
        return AttackEvent::Shoot;
    }

    Zone nextZone = st->to;

    if (st->isLongBall) {
        if (nextZone == Zone::A2 && (ballZone == Zone::A1 || ballZone == Zone::A3)) {
            return AttackEvent::Cross;
        }
        else {
            return AttackEvent::LongPass;
        }
    }

    return AttackEvent::ShortPass;
}

Zone Tactic::GetNextZone(Zone ballZone) const {
    if (_chosenPath.steps.empty()) {
        return ballZone;
    }

    auto findZoneStep = [&](const ZoneStep& step) {
        return step.from == ballZone;
        };

    auto st = std::find_if(_chosenPath.steps.begin(), _chosenPath.steps.end(), findZoneStep);

    if (st == _chosenPath.steps.end()) {
        return ballZone;
    }

    return st->to;
}

DefenseTactic::DefenseTactic() : _defenseTacticType{ DefenseTacticType::Pressing } {
}

DefenseTactic::DefenseTactic(DefenseTacticType type) : _defenseTacticType{ type } {
    switch (type) {
        using enum DefenseTacticType;
    case Pressing:
        *this = TacticFactory::CreatePressing();
        break;
    case ManMarking:
        *this = TacticFactory::CreateManMarking();
        break;
    case ZonalMarking:
        *this = TacticFactory::CreateZonalMarking();
        break;
    case CounterPressing:
        *this = TacticFactory::CreateCounterPressing();
        break;
    default:
        *this = TacticFactory::CreatePressing();
        break;
    }
}

DefenseTacticType DefenseTactic::getDefenseTacticType() const {
    return _defenseTacticType;
}

void DefenseTactic::setDefenseTacticType(DefenseTacticType type) {
    _defenseTacticType = type;
}

AttackEvent Tactic::StartAttack(Zone ballZone) {
    // TODO - More initialization will be done later

    // Whenever the ball is in possession of a team after either kickoff or a successful defensive action,
    // we will call this function to start the attack
    if (!_newAttackPathChosen) {
        _chosenPath = GetRandomPath(ballZone);
        _newAttackPathChosen = true;
    }

    return GetAttackEvent(ballZone);
}

void Tactic::ResetAttack() {
    _chosenPath = ZonePath{};
    _newAttackPathChosen = false;
}

DefenseEvent DefenseTactic::RespondToAttack(AttackEvent attackEvent) {
    // TODO - Implement this function based on the chosen tactic and the attack event
    return DefenseEvent::Pressure; // Placeholder return value
}
