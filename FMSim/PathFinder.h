#pragma once
#include <vector>
#include "MatchDefinitions.h"
#include "ZoneGraph.h"

class PathFinder {
public:
    static std::vector<Zone> FindPath(
        const ZoneGraph& graph,
        Zone startZone,
        Zone targetZone);
};
