//
// Created by igork on 22.03.2024.
//

#ifndef ZTGK_ASTARPATHFINDING_H
#define ZTGK_ASTARPATHFINDING_H

#include <vector>
#include "VectorUtils.h"
#include "ECS/Grid/Grid.h"
#include <unordered_set>


using namespace std;



class AstarPathfinding {
public:
    std::vector<Vector2Int> path;


    // Constructors
    AstarPathfinding() = default;
    explicit AstarPathfinding(Grid* grid, Unit* unit);


    // Destructor
    ~AstarPathfinding() = default;

    // Most important method
    std::vector<Vector2Int> FindPath(Vector2Int start, Vector2Int target,  std::vector<Vector2Int> forbiddenTiles = {}, bool debug = false);

    std::vector<Vector2Int> FindPath(Vector2Int start, Vector2Int target, int max_iterations);

    Vector2Int GetNearestVacantTile(Vector2Int target, Vector2Int origin);

    [[deprecated("Works, but shitty as hell")]] Vector2Int old_GetNearestVacantTile(Vector2Int target, Vector2Int origin) ;

    // Potrzebowalem bez sprawdzania == origin XOXO
    Vector2Int GetNearestVacantTileAround(Vector2Int origin, std::vector<Vector2Int> forbiddenTiles);

    Vector2Int GetNearestVacantTileInRange(Vector2Int target, Vector2Int origin, int range);
private:
    Grid* grid;
    Unit* unit;
    //Helper functions
    Vector2Int GetLowestFScore(std::unordered_set<Vector2Int>& openSet, std::unordered_map<Vector2Int, float>& fScore);
    std::vector<Vector2Int> ReconstructPath(std::unordered_map<Vector2Int, Vector2Int>& cameFrom, Vector2Int current);
    std::vector<Vector2Int> GetNeighbours(Vector2Int current, bool simple = false);

};



#endif //ZTGK_ASTARPATHFINDING_H
