#ifndef SIMULATION_ENGINE_H
#define SIMULATION_ENGINE_H

#include <vector>
#include <cmath>
#include <random>
#include "Agent.h"

class SimulationEngine {
public:
    SimulationEngine();

    void updateTick();

    // God Mode methods
    void spawnAgentAt(float x, float y);
    void spawnFoodAt(float x, float y);
    void addZone(float x, float y, float radius, ZoneType type);
    void clearAllZones();

    // Settings
    void setMutationRate(float rate) { globalMutationRate = rate; }
    void setSimSpeed(float speed) { simSpeed = speed; }

    [[nodiscard]] const std::vector<Agent>& getPopulation() const { return population; }
    [[nodiscard]] std::vector<Agent>& getPopulation() { return population; }
    [[nodiscard]] const std::vector<Zone>& getZones() const { return zones; }
    [[nodiscard]] const std::vector<Food>& getFood() const { return foodList; }

private:
    std::vector<Agent> population;
    std::vector<Food> foodList;
    std::vector<Zone> zones;

    const float WORLD_WIDTH = 1080.0f;
    const float WORLD_HEIGHT = 2000.0f;

    std::mt19937 rng;
    float globalMutationRate = 0.05f;
    float simSpeed = 1.0f;

    void spawnFood(int amount);
    float getDistance(float x1, float y1, float x2, float y2) const;

    // --- SPATIAL GRID OPTIMIZATION ---
    static constexpr float CELL_SIZE = 100.0f;
    static constexpr int GRID_COLS = 12;
    static constexpr int GRID_ROWS = 22;

    std::vector<Agent*> agentGrid[GRID_COLS][GRID_ROWS];
    std::vector<Food*> foodGrid[GRID_COLS][GRID_ROWS];

    void clearGrid();
    void populateGrid();
    std::vector<Agent*> getAgentsInRadius(float x, float y, float radius);
    std::vector<Food*> getFoodInRadius(float x, float y, float radius);
};

#endif // SIMULATION_ENGINE_H
