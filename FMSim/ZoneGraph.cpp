#include "ZoneGraph.h"

ZoneGraph::ZoneGraph()
{
    adjacency = {
        { Zone::H1, {Zone::H2, Zone::M1 } },
        { Zone::H2, {Zone::H1, Zone::H3, Zone::M2 } },
        { Zone::H3, {Zone::H2, Zone::M3 } },

        { Zone::M1, {Zone::H1, Zone::A1, Zone::M2 } },
        { Zone::M2, {Zone::M1, Zone::M3, Zone::H2, Zone::A2}},
        { Zone::M3, {Zone::H3, Zone::A3, Zone::M2 } },

        { Zone::A1, {Zone::A2, Zone::M1 } },
        { Zone::A2, {Zone::A1, Zone::A3, Zone::M2 } },
        { Zone::A3, {Zone::A2, Zone::M3 } },
    };
}

const std::vector<Zone>& ZoneGraph::GetAdjacent(Zone zone) const
{
    return adjacency.at(zone);
}