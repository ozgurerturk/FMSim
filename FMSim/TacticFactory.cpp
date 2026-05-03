#include "TacticFactory.h"
#include <map>
#include "PathFinder.h"

static ZonePath ConvertZonesToPath(const std::vector<Zone>& zones) {
    ZonePath path;

    if (zones.size() < 2) {
        return path;
    }

    for (size_t i = 0; i < zones.size() - 1; i++) {
        path.steps.push_back(ZoneStep{ zones[i], zones[i + 1] });
    }

    return path;
}

Tactic TacticFactory::CreateWingPlay() {
    Tactic tactic;
    tactic.setTacticType(TacticType::WingPlay);

    tactic.setPaths(
    {
        // H2 -> M2 -> M1 -> A1 -> A2
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::M1 },
                ZoneStep{ Zone::M1, Zone::A1 },
                ZoneStep{ Zone::A1, Zone::A2 },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> M2 -> M1 -> A1 -> A2 (A1 -> A2 long / cross)
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::M1 },
                ZoneStep{ Zone::M1, Zone::A1 },
                ZoneStep{ Zone::A1, Zone::A2, true },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> M2 -> M3 -> A3 -> A2
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::M3 },
                ZoneStep{ Zone::M3, Zone::A3 },
                ZoneStep{ Zone::A3, Zone::A2 },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> M2 -> M3 -> A3 -> A2 (A3 -> A2 long / cross)
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::M3 },
                ZoneStep{ Zone::M3, Zone::A3 },
                ZoneStep{ Zone::A3, Zone::A2, true },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> H1 -> M1 -> A1 -> A2
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::H1 },
                ZoneStep{ Zone::H1, Zone::M1 },
                ZoneStep{ Zone::M1, Zone::A1 },
                ZoneStep{ Zone::A1, Zone::A2 },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> H1 -> M1 -> A1 -> A2 (A1 -> A2 long / cross)
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::H1 },
                ZoneStep{ Zone::H1, Zone::M1 },
                ZoneStep{ Zone::M1, Zone::A1 },
                ZoneStep{ Zone::A1, Zone::A2, true },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> H3 -> M3 -> A3 -> A2
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::H3 },
                ZoneStep{ Zone::H3, Zone::M3 },
                ZoneStep{ Zone::M3, Zone::A3 },
                ZoneStep{ Zone::A3, Zone::A2 },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> H3 -> M3 -> A3 -> A2 (A3 -> A2 long / cross)
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::H3 },
                ZoneStep{ Zone::H3, Zone::M3 },
                ZoneStep{ Zone::M3, Zone::A3 },
                ZoneStep{ Zone::A3, Zone::A2, true },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> M2 -> A1 -> A2   (M2 -> A1 long)
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::A1, true },
                ZoneStep{ Zone::A1, Zone::A2 },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> M2 -> A1 -> A2   (M2 -> A1 long, A1 -> A2 long / cross)
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::A1, true },
                ZoneStep{ Zone::A1, Zone::A2, true },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> M2 -> A3 -> A2   (M2 -> A3 long)
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::A3, true },
                ZoneStep{ Zone::A3, Zone::A2 },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> M2 -> A3 -> A2   (M2 -> A3 long, A3 -> A2 long / cross)
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::A3, true },
                ZoneStep{ Zone::A3, Zone::A2, true },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        }
    });

    return tactic;
}

Tactic TacticFactory::CreateLongBall() {
    Tactic tactic;
    tactic.setTacticType(TacticType::LongBall);

    tactic.setPaths(
    {
        // H2 -> A2(L)
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::A2, true },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> A1(L) -> A2
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::A1, true },
                ZoneStep{ Zone::A1, Zone::A2 },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },
        // H2 -> A1(L) -> A2(L)
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::A1, true },
                ZoneStep{ Zone::A1, Zone::A2, true },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },
        // H2 -> A3(L) -> A2
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::A3, true },
                ZoneStep{ Zone::A3, Zone::A2 },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },
        // H2 -> A3(L) -> A2(L)
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::A3, true },
                ZoneStep{ Zone::A3, Zone::A2, true },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        }
    });

    return tactic;
}

Tactic TacticFactory::CreateTikiTaka() {
    Tactic tactic;
    tactic.setTacticType(TacticType::TikiTaka);

    tactic.setPaths(
    {
        // H2 -> M2 <=> A2
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::A2, false, true },
                ZoneStep{ Zone::A2, Zone::M2, false, true },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> M2 -> A1 -> A2
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::A1 },
                ZoneStep{ Zone::A1, Zone::A2 },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> M2 -> A3 -> A2
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::A3 },
                ZoneStep{ Zone::A3, Zone::A2 },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> M2 -> M1 -> A2
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::M1 },
                ZoneStep{ Zone::M1, Zone::A2 },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> M2 -> M3 -> A2
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::M3 },
                ZoneStep{ Zone::M3, Zone::A2 },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        }
    });

    return tactic;
}

Tactic TacticFactory::CreateCounterAttack() {
    Tactic tactic;
    tactic.setTacticType(TacticType::CounterAttack);

    tactic.setPaths(
    {
        // H2 -> M2 -> A2
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::A2 },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> M2(L) -> M1 -> A2(L)
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2, true },
                ZoneStep{ Zone::M2, Zone::M1 },
                ZoneStep{ Zone::M1, Zone::A2, true },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> M2(L) -> M3 -> A2(L)
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2, true },
                ZoneStep{ Zone::M2, Zone::M3 },
                ZoneStep{ Zone::M3, Zone::A2, true },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> M2 -> A1(L) -> A2
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::A1, true },
                ZoneStep{ Zone::A1, Zone::A2 },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        },

        // H2 -> M2 -> A3(L) -> A2
        ZonePath
        {
            {
                ZoneStep{ Zone::H2, Zone::M2 },
                ZoneStep{ Zone::M2, Zone::A3, true },
                ZoneStep{ Zone::A3, Zone::A2 },
                ZoneStep{ Zone::A2, Zone::A2 }
            }
        }
    });

    return tactic;
}

// Default tactic is Possession Game because it's the most basic and versatile tactic that can be used in various situations.
Tactic TacticFactory::CreatePossessionGame() {
    Tactic tactic;
    tactic.setTacticType(TacticType::Possession);

    ZoneGraph graph;

    Zone startZone = Zone::H2;
    Zone targetZone = Zone::A2;

    auto zoneSequence = PathFinder::FindPath(graph, startZone, targetZone);
    auto zonePath = ConvertZonesToPath(zoneSequence);

    tactic.addPath(zonePath);

    // Add ZoneStep{ Zone::A2, Zone::A2 } to the last step to indicate that the attack ends in the A2 zone.
    // This is necessary because the GetAttackEvent method in the Tactic class relies on the last step of the chosen path to determine the final zone of the attack.
    // Will now attempt to shoot or dribble in the A2 zone after reaching it, instead of just standing there without any action.

    if (tactic.hasPaths()) {
        tactic.addStepToLastPath(ZoneStep{ Zone::A2, Zone::A2 });
    }

    return tactic;
}

DefenseTactic TacticFactory::CreatePressing() {
    DefenseTactic tactic;
    tactic.setDefenseTacticType(DefenseTacticType::Pressing);
    return tactic;
}

DefenseTactic TacticFactory::CreateManMarking() {
    DefenseTactic tactic;
    tactic.setDefenseTacticType(DefenseTacticType::ManMarking);
    return tactic;
}

DefenseTactic TacticFactory::CreateZonalMarking() {
    DefenseTactic tactic;
    tactic.setDefenseTacticType(DefenseTacticType::ZonalMarking);
    return tactic;
}

DefenseTactic TacticFactory::CreateCounterPressing() {
    DefenseTactic tactic;
    tactic.setDefenseTacticType(DefenseTacticType::CounterPressing);
    return tactic;
}
