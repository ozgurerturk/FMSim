#include "PathFinder.h"
#include <queue>
#include <random>

// Breadth-First Search but with randomizer
std::vector<Zone> PathFinder::FindPath(const ZoneGraph& graph,
    Zone startZone,
    Zone targetZone) {
    // Random number generator setup
    std::random_device rd;
    std::mt19937 g(rd());

    std::vector<Zone> path = {};

    std::queue<Zone> queue;
    std::map<Zone, bool> visited;
    std::map<Zone, Zone> parent;

    queue.push(startZone);
    visited[startZone] = true;

    while (!queue.empty()) {
        Zone curr = queue.front();
        queue.pop();

        if (curr == targetZone) {
            break;
        }

        // Shuffle the adjacent zones to introduce randomness in the pathfinding
        std::vector<Zone> adjacentZones = graph.GetAdjacent(curr);
        std::vector<Zone> shuffled = adjacentZones;

        std::shuffle(shuffled.begin(), shuffled.end(), g);

        // Explore the adjacent zones in random order
        for (const auto& next : shuffled) {
            if (!visited[next]) {
                visited[next] = true;
                parent[next] = curr;
                queue.push(next);
            }
        }
    }

    if (!visited[targetZone]) {
        return {};
    }

    for (auto at = targetZone; at != startZone; at = parent[at]) {
        path.push_back(at);
    }

    path.push_back(startZone);
    std::reverse(path.begin(), path.end());

    return path;
};
