#pragma once
#include <map>
#include <vector>

enum class Position {
    Goalkeeper,
    Defender,
    Midfielder,
    Attacker
};

enum class Zone {
    H1, H2, H3,
    M1, M2, M3,
    A1, A2, A3
};

enum class TacticType {
    Possession,
    WingPlay,
    LongBall,
    TikiTaka,
    CounterAttack
};

enum class DefenseTacticType {
    Pressing,
    ManMarking,
    ZonalMarking,
    CounterPressing
};

enum class EventType {
    Normal,
    Critical
};
