#include "Tactics.h"
#include <cstdlib>
#include "TacticFactory.h"
#include "PathFinder.h"
#include "ZoneGraph.h"
#include <ranges>
#include <algorithm>

Tactic::Tactic()
    : tacticType(TacticType::Possession), _tacticType(TacticType::Possession)
{
}

Tactic::Tactic(TacticType type)
    : tacticType(type), _tacticType(type)
{
    switch (type)
    {
    case TacticType::WingPlay:
        *this = TacticFactory::CreateWingPlay();
        break;
    case TacticType::LongBall:
        *this = TacticFactory::CreateLongBall();
        break;
    case TacticType::TikiTaka:
        *this = TacticFactory::CreateTikiTaka();
        break;
    case TacticType::CounterAttack:
        *this = TacticFactory::CreateCounterAttack();
        break;
    case TacticType::Possession:
    default:
        *this = TacticFactory::CreatePossessionGame();
        break;
    }
}

ZonePath Tactic::GetRandomPath(Zone ballZone) const
{
    if (paths.empty())
        return ZonePath();

    std::vector<ZonePath> candidates;
    for (const auto& path : paths)
    {
        auto st = std::find_if(path.steps.begin(), path.steps.end(), [&](const ZoneStep& step) {
            return step.from == ballZone;
            });

        if (st == path.steps.end())
        {
            continue;
        }

        ZonePath candidate;
        candidate.steps.assign(st, path.steps.end());
        candidates.push_back(candidate);
    }

    if (candidates.empty())
    {
        if (ballZone == Zone::A2)
        {
            ZonePath shootPath;
            shootPath.steps.push_back(ZoneStep{ Zone::A2, Zone::A2 });
            return shootPath;
        }

        ZoneGraph graph;
        auto fallbackZones = PathFinder::FindPath(graph, ballZone, Zone::A2);
        ZonePath fallbackPath;

        if (fallbackZones.size() >= 2)
        {
            for (size_t i = 0; i + 1 < fallbackZones.size(); ++i)
            {
                fallbackPath.steps.push_back(ZoneStep{ fallbackZones[i], fallbackZones[i + 1] });
            }

            fallbackPath.steps.push_back(ZoneStep{ Zone::A2, Zone::A2 });
        }

        return fallbackPath;
    }

    int idx = rand() % candidates.size();
    return candidates[idx];
}

AttackEvent Tactic::GetAttackEvent(Zone ballZone) const
{
    if (chosenPath.steps.empty())
    {
        return AttackEvent::Clearance;
    }

    auto findZoneStep = [&](const ZoneStep& step) {
        return step.from == ballZone;
        };

    auto st = std::find_if(chosenPath.steps.begin(), chosenPath.steps.end(), findZoneStep);

    if (st == chosenPath.steps.end())
    {
        return AttackEvent::Clearance;
    }

    if (st->from == Zone::A2 && st->to == Zone::A2)
    {
        // TODO - More complex logic will be implemented here later to determine whether to
        // shoot, dribble or back pass based on the situation, player attributes and other factors.
        return AttackEvent::Shoot;
    }

    Zone nextZone = st->to;

    if (st->isLongBall)
    {
        if (nextZone == Zone::A2 && (ballZone == Zone::A1 || ballZone == Zone::A3))
        {
            return AttackEvent::Cross;
        }
        else
        {
            return AttackEvent::LongPass;
        }
    }

    return AttackEvent::ShortPass;
}

Zone Tactic::GetNextZone(Zone ballZone) const
{
    if (chosenPath.steps.empty())
    {
        return ballZone;
    }

    auto findZoneStep = [&](const ZoneStep& step) {
        return step.from == ballZone;
        };

    auto st = std::find_if(chosenPath.steps.begin(), chosenPath.steps.end(), findZoneStep);

    if (st == chosenPath.steps.end())
    {
        return ballZone;
    }

    return st->to;
}

DefenseTactic::DefenseTactic() : defenseTacticType{ DefenseTacticType::Pressing }
{
}

DefenseTactic::DefenseTactic(DefenseTacticType type) : defenseTacticType{ type }
{
    switch (type)
    {
    case DefenseTacticType::Pressing:
        *this = TacticFactory::CreatePressing();
        break;
    case DefenseTacticType::ManMarking:
        *this = TacticFactory::CreateManMarking();
        break;
    case DefenseTacticType::ZonalMarking:
        *this = TacticFactory::CreateZonalMarking();
        break;
    case DefenseTacticType::CounterPressing:
        *this = TacticFactory::CreateCounterPressing();
        break;
    default:
        *this = TacticFactory::CreatePressing();
        break;
	}
}

AttackEvent Tactic::StartAttack(Zone ballZone)
{
    // TODO - More initialization will be done later

	// Whenever the ball is in possession of a team after either kickoff or a successful defensive action, 
    // we will call this function to start the attack
    if (newAttackPathChosen == false)
    {
        chosenPath = GetRandomPath(ballZone);
        newAttackPathChosen = true;
    }

	return GetAttackEvent(ballZone);
}

void Tactic::ResetAttack()
{
    chosenPath = ZonePath{};
    newAttackPathChosen = false;
}

DefenseEvent DefenseTactic::RespondToAttack(AttackEvent attackEvent)
{
    // TODO - Implement this function based on the chosen tactic and the attack event
    return DefenseEvent::Pressure; // Placeholder return value
}
