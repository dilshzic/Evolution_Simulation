#ifndef SPATIAL_GRID_H
#define SPATIAL_GRID_H

#include <vector>
#include <algorithm>
#include "Agent.h"

class SpatialGrid {
public:
    static constexpr float CELL_SIZE = 100.0f;
    static constexpr int GRID_COLS = 12;
    static constexpr int GRID_ROWS = 22;

    void clear() {
        for (int x = 0; x < GRID_COLS; ++x) {
            for (int y = 0; y < GRID_ROWS; ++y) {
                agentGrid[x][y].clear();
                foodGrid[x][y].clear();
            }
        }
    }

    void populate(std::vector<Agent>& population, std::vector<Food>& foodList) {
        for (auto& agent : population) {
            if (!agent.isAlive) continue;
            int gridX = std::clamp(static_cast<int>(agent.x / CELL_SIZE), 0, GRID_COLS - 1);
            int gridY = std::clamp(static_cast<int>(agent.y / CELL_SIZE), 0, GRID_ROWS - 1);
            agentGrid[gridX][gridY].push_back(&agent);
        }
        for (auto& food : foodList) {
            if (food.isEaten) continue;
            int gridX = std::clamp(static_cast<int>(food.x / CELL_SIZE), 0, GRID_COLS - 1);
            int gridY = std::clamp(static_cast<int>(food.y / CELL_SIZE), 0, GRID_ROWS - 1);
            foodGrid[gridX][gridY].push_back(&food);
        }
    }

    std::vector<Agent*> getAgentsInRadius(float x, float y, float radius) {
        std::vector<Agent*> result;
        int minCol = std::clamp(static_cast<int>((x - radius) / CELL_SIZE), 0, GRID_COLS - 1);
        int maxCol = std::clamp(static_cast<int>((x + radius) / CELL_SIZE), 0, GRID_COLS - 1);
        int minRow = std::clamp(static_cast<int>((y - radius) / CELL_SIZE), 0, GRID_ROWS - 1);
        int maxRow = std::clamp(static_cast<int>((y + radius) / CELL_SIZE), 0, GRID_ROWS - 1);

        for (int c = minCol; c <= maxCol; ++c) {
            for (int r = minRow; r <= maxRow; ++r) {
                result.insert(result.end(), agentGrid[c][r].begin(), agentGrid[c][r].end());
            }
        }
        return result;
    }

    std::vector<Food*> getFoodInRadius(float x, float y, float radius) {
        std::vector<Food*> result;
        int minCol = std::clamp(static_cast<int>((x - radius) / CELL_SIZE), 0, GRID_COLS - 1);
        int maxCol = std::clamp(static_cast<int>((x + radius) / CELL_SIZE), 0, GRID_COLS - 1);
        int minRow = std::clamp(static_cast<int>((y - radius) / CELL_SIZE), 0, GRID_ROWS - 1);
        int maxRow = std::clamp(static_cast<int>((y + radius) / CELL_SIZE), 0, GRID_ROWS - 1);

        for (int c = minCol; c <= maxCol; ++c) {
            for (int r = minRow; r <= maxRow; ++r) {
                result.insert(result.end(), foodGrid[c][r].begin(), foodGrid[c][r].end());
            }
        }
        return result;
    }

private:
    std::vector<Agent*> agentGrid[GRID_COLS][GRID_ROWS];
    std::vector<Food*> foodGrid[GRID_COLS][GRID_ROWS];
};

#endif // SPATIAL_GRID_H
