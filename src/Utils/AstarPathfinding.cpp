//
// Created by igork on 22.03.2024.
//

#include "AstarPathfinding.h"

/**
 * @brief AstarPathfinding constructor
 * @param grid
 */
AstarPathfinding::AstarPathfinding(Grid *grid) {
    this->grid = grid;
}

/**
 * @brief Uses A* algorithm to find the shortest path
 * from start tile to target tile. \n
 * Stores the path in the \b path member variable
 * @param start
 * @param target
 */
void AstarPathfinding::FindPath(Vector2Int start, Vector2Int target) {

    std::unordered_set<Vector2Int> openSet;
    std::unordered_set<Vector2Int> closedSet;
    std::unordered_map<Vector2Int, Vector2Int> cameFrom;
    std::unordered_map<Vector2Int, float> gScore;
    std::unordered_map<Vector2Int, float> fScore;

    openSet.insert(start);
    gScore[start] = 0;
    fScore[start] = VectorUtils::Distance(start, target);

    while(!openSet.empty()){

        Vector2Int current = GetLowestFScore(openSet, fScore);
        if (current == target){
            path = ReconstructPath(cameFrom, current);
            return;
        }

        openSet.erase(current);
        closedSet.insert(current);

        for(auto neigh : GetNeighbours(current)){
            if(closedSet.contains(neigh)){
                continue;
            }

            float tentativeGScore = gScore[current] + VectorUtils::Distance(current, neigh);
            if(!openSet.contains(neigh) || tentativeGScore < gScore[neigh]){
                cameFrom[neigh] = current;
                gScore[neigh] = tentativeGScore;
                fScore[neigh] = gScore[neigh] + VectorUtils::Distance(neigh, target);
                if(!openSet.contains(neigh)){
                    openSet.insert(neigh);
                }
            }

        }


    }
}
//________________________________________________HELPER FUNCTIONS_____________________________________________________
/**
 * @brief Returns the node with the lowest fScore
 * @param openSet
 * @param fScore
 * @return Vector2Int
 */
Vector2Int
AstarPathfinding::GetLowestFScore(unordered_set<Vector2Int> &openSet, unordered_map<Vector2Int, float> &fScore) {
    Vector2Int lowest = Vector2Int(0,0);
    float lowestFScore = std::numeric_limits<float>::max();

    for(auto &node : openSet){
        if(fScore[node] < lowestFScore){
            lowestFScore = fScore[node];
            lowest = node;
        }
    }
    return lowest;
}

/**
 * @brief Reconstructs the path from the cameFrom map and returns it
 * @param cameFrom
 * @param current
 * @return std::vector<Vector2Int>
 */
std::vector<Vector2Int>
AstarPathfinding::ReconstructPath(unordered_map<Vector2Int, Vector2Int> &cameFrom, Vector2Int current) {
    std::vector<Vector2Int> totalPath;
    totalPath.push_back(current);

    while(cameFrom.find(current) != cameFrom.end()){
        current = cameFrom[current];
        totalPath.push_back(current);
    }

    return totalPath;
}


/**
 * @brief Returns the neighbours of the current node \n
 *
 * @param current - the current node
 * @param simple - default \b false -> searches for diagonal neighbours too \n
 *                      \b true -> only searches for up, down, left, right neighbours
 *
 * @return std::vector<Vector2Int>
 */
std::vector<Vector2Int> AstarPathfinding::GetNeighbours(Vector2Int current, bool simple) {
    std::vector<Vector2Int> neighbours;

    Vector2Int right = current + Vector2Int(1, 0);
    Vector2Int left = current + Vector2Int(-1, 0);
    Vector2Int up = current + Vector2Int(0, 1);
    Vector2Int down = current + Vector2Int(0, -1);


    //right
    if(grid->getTileAt(right) != nullptr && grid->getTileAt(right)->vacant){
        neighbours.push_back(right);
    }

    //left
    if(grid->getTileAt(left) != nullptr && grid->getTileAt(left)->vacant){
        neighbours.push_back(left);
    }

    //up
    if(grid->getTileAt(up) != nullptr && grid->getTileAt(up)->vacant){
        neighbours.push_back(up);
    }

    //down
    if(grid->getTileAt(down) != nullptr && grid->getTileAt(down)->vacant){
        neighbours.push_back(down);
    }

    if(simple){
        return neighbours;
    }
    //UpRight
    if((grid->getTileAt(right) != nullptr && grid->getTileAt(right)->vacant) && (grid->getTileAt(up) != nullptr && grid->getTileAt(up)->vacant)){
        Vector2Int upRight = current + Vector2Int(1, 1);
        if(grid->getTileAt(upRight) != nullptr && grid->getTileAt(upRight)->vacant){
            neighbours.push_back(upRight);
        }
    }

    //DownRight
    if((grid->getTileAt(right) != nullptr && grid->getTileAt(right)->vacant) && (grid->getTileAt(down) != nullptr && grid->getTileAt(down)->vacant)){
        Vector2Int downRight = current + Vector2Int(1, -1);
        if(grid->getTileAt(downRight) != nullptr && grid->getTileAt(downRight)->vacant){
            neighbours.push_back(downRight);
        }
    }

    //UpLeft
    if((grid->getTileAt(left) != nullptr && grid->getTileAt(left)->vacant) && (grid->getTileAt(up) != nullptr && grid->getTileAt(up)->vacant)){
        Vector2Int upLeft = current + Vector2Int(-1, 1);
        if(grid->getTileAt(upLeft) != nullptr && grid->getTileAt(upLeft)->vacant){
            neighbours.push_back(upLeft);
        }
    }

    //DownLeft
    if((grid->getTileAt(left) != nullptr && grid->getTileAt(left)->vacant) && (grid->getTileAt(down) != nullptr && grid->getTileAt(down)->vacant)){
        Vector2Int downLeft = current + Vector2Int(-1, -1);
        if(grid->getTileAt(downLeft) != nullptr && grid->getTileAt(downLeft)->vacant){
            neighbours.push_back(downLeft);
        }
    }

    return neighbours;
}