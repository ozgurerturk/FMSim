#pragma once
#include <vector>
#include <map>
#include "MatchDefinitions.h"

class ZoneGraph
{
public:
    ZoneGraph();
    const std::vector<Zone>& GetAdjacent(Zone zone) const;

private:
    std::map<Zone, std::vector<Zone>> adjacency;
};