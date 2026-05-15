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
    using enum Zone;
    Tactic tactic;
    tactic.setTacticType(TacticType::WingPlay);

    tactic.setPaths(
        {
            // H2 -> M2 -> M1 -> A1 -> A2
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, M1 },
                    ZoneStep{ M1, A1 },
                    ZoneStep{ A1, A2 },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> M2 -> M1 -> A1 -> A2 (A1 -> A2 long / cross)
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, M1 },
                    ZoneStep{ M1, A1 },
                    ZoneStep{ A1, A2, true },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> M2 -> M3 -> A3 -> A2
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, M3 },
                    ZoneStep{ M3, A3 },
                    ZoneStep{ A3, A2 },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> M2 -> M3 -> A3 -> A2 (A3 -> A2 long / cross)
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, M3 },
                    ZoneStep{ M3, A3 },
                    ZoneStep{ A3, A2, true },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> H1 -> M1 -> A1 -> A2
            ZonePath
            {
                {
                    ZoneStep{ H2, H1 },
                    ZoneStep{ H1, M1 },
                    ZoneStep{ M1, A1 },
                    ZoneStep{ A1, A2 },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> H1 -> M1 -> A1 -> A2 (A1 -> A2 long / cross)
            ZonePath
            {
                {
                    ZoneStep{ H2, H1 },
                    ZoneStep{ H1, M1 },
                    ZoneStep{ M1, A1 },
                    ZoneStep{ A1, A2, true },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> H3 -> M3 -> A3 -> A2
            ZonePath
            {
                {
                    ZoneStep{ H2, H3 },
                    ZoneStep{ H3, M3 },
                    ZoneStep{ M3, A3 },
                    ZoneStep{ A3, A2 },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> H3 -> M3 -> A3 -> A2 (A3 -> A2 long / cross)
            ZonePath
            {
                {
                    ZoneStep{ H2, H3 },
                    ZoneStep{ H3, M3 },
                    ZoneStep{ M3, A3 },
                    ZoneStep{ A3, A2, true },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> M2 -> A1 -> A2   (M2 -> A1 long)
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, A1, true },
                    ZoneStep{ A1, A2 },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> M2 -> A1 -> A2   (M2 -> A1 long, A1 -> A2 long / cross)
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, A1, true },
                    ZoneStep{ A1, A2, true },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> M2 -> A3 -> A2   (M2 -> A3 long)
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, A3, true },
                    ZoneStep{ A3, A2 },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> M2 -> A3 -> A2   (M2 -> A3 long, A3 -> A2 long / cross)
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, A3, true },
                    ZoneStep{ A3, A2, true },
                    ZoneStep{ A2, A2 }
                }
            }
        });

    return tactic;
}

Tactic TacticFactory::CreateLongBall() {
    using enum Zone;
    Tactic tactic;
    tactic.setTacticType(TacticType::LongBall);

    tactic.setPaths(
        {
            // H2 -> A2(L)
            ZonePath
            {
                {
                    ZoneStep{ H2, A2, true },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> A1(L) -> A2
            ZonePath
            {
                {
                    ZoneStep{ H2, A1, true },
                    ZoneStep{ A1, A2 },
                    ZoneStep{ A2, A2 }
                }
            },
            // H2 -> A1(L) -> A2(L)
            ZonePath
            {
                {
                    ZoneStep{ H2, A1, true },
                    ZoneStep{ A1, A2, true },
                    ZoneStep{ A2, A2 }
                }
            },
            // H2 -> A3(L) -> A2
            ZonePath
            {
                {
                    ZoneStep{ H2, A3, true },
                    ZoneStep{ A3, A2 },
                    ZoneStep{ A2, A2 }
                }
            },
            // H2 -> A3(L) -> A2(L)
            ZonePath
            {
                {
                    ZoneStep{ H2, A3, true },
                    ZoneStep{ A3, A2, true },
                    ZoneStep{ A2, A2 }
                }
            }
        });

    return tactic;
}

Tactic TacticFactory::CreateTikiTaka() {
    using enum Zone;
    Tactic tactic;
    tactic.setTacticType(TacticType::TikiTaka);

    tactic.setPaths(
        {
            // H2 -> M2 <=> A2
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, A2, false, true },
                    ZoneStep{ A2, M2, false, true },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> M2 -> A1 -> A2
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, A1 },
                    ZoneStep{ A1, A2 },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> M2 -> A3 -> A2
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, A3 },
                    ZoneStep{ A3, A2 },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> M2 -> M1 -> A2
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, M1 },
                    ZoneStep{ M1, A2 },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> M2 -> M3 -> A2
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, M3 },
                    ZoneStep{ M3, A2 },
                    ZoneStep{ A2, A2 }
                }
            }
        });

    return tactic;
}

Tactic TacticFactory::CreateCounterAttack() {
    using enum Zone;
    Tactic tactic;
    tactic.setTacticType(TacticType::CounterAttack);

    tactic.setPaths(
        {
            // H2 -> M2 -> A2
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, A2 },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> M2(L) -> M1 -> A2(L)
            ZonePath
            {
                {
                    ZoneStep{ H2, M2, true },
                    ZoneStep{ M2, M1 },
                    ZoneStep{ M1, A2, true },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> M2(L) -> M3 -> A2(L)
            ZonePath
            {
                {
                    ZoneStep{ H2, M2, true },
                    ZoneStep{ M2, M3 },
                    ZoneStep{ M3, A2, true },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> M2 -> A1(L) -> A2
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, A1, true },
                    ZoneStep{ A1, A2 },
                    ZoneStep{ A2, A2 }
                }
            },

            // H2 -> M2 -> A3(L) -> A2
            ZonePath
            {
                {
                    ZoneStep{ H2, M2 },
                    ZoneStep{ M2, A3, true },
                    ZoneStep{ A3, A2 },
                    ZoneStep{ A2, A2 }
                }
            }
        });

    return tactic;
}

// Default tactic is Possession Game because it's the most basic and versatile tactic that can be used in various situations.
Tactic TacticFactory::CreatePossessionGame() {
    using enum Zone;
    Tactic tactic;
    tactic.setTacticType(TacticType::Possession);

    ZoneGraph graph;

    Zone startZone = H2;
    Zone targetZone = A2;

    auto zoneSequence = PathFinder::FindPath(graph, startZone, targetZone);
    auto zonePath = ConvertZonesToPath(zoneSequence);

    tactic.addPath(zonePath);

    // Add ZoneStep{ Zone::A2, Zone::A2 } to the last step to indicate that the attack ends in the A2 zone.
    // This is necessary because the GetAttackEvent method in the Tactic class relies on the last step of the chosen path to determine the final zone of the attack.
    // Will now attempt to shoot or dribble in the A2 zone after reaching it, instead of just standing there without any action.

    if (tactic.hasPaths()) {
        tactic.addStepToLastPath(ZoneStep{ A2, A2 });
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
