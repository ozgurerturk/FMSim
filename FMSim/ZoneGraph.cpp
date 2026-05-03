#include "ZoneGraph.h"

ZoneGraph::ZoneGraph() {
    using enum Zone;
    adjacency = {
        { H1, {H2, M1 } },
        { H2, {H1, H3, M2 } },
        { H3, {H2, M3 } },

        { M1, {H1, A1, M2 } },
        { M2, {M1, M3, H2, A2}},
        { M3, {H3, A3, M2 } },

        { A1, {A2, M1 } },
        { A2, {A1, A3, M2 } },
        { A3, {A2, M3 } },
    };
}

const std::vector<Zone>& ZoneGraph::GetAdjacent(Zone zone) const {
    return adjacency.at(zone);
}
