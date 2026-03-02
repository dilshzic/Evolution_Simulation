#include "SimulationEngine.h"
#include <algorithm>

SimulationEngine::SimulationEngine() {
    std::random_device rd;
    rng = std::mt19937(rd());
    spawnFood(150);
}

void SimulationEngine::spawnFood(int amount) {
    std::uniform_real_distribution<float> distX(0.0f, WORLD_WIDTH);
    std::uniform_real_distribution<float> distY(0.0f, WORLD_HEIGHT);
    for (int i = 0; i < amount; ++i) {
        foodList.push_back({distX(rng), distY(rng), 30.0f, false});
    }
}

float SimulationEngine::getDistance(float x1, float y1, float x2, float y2) const {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return std::sqrt(dx * dx + dy * dy);
}

void SimulationEngine::spawnAgentAt(float x, float y) {
    Agent a;
    a.x = x;
    a.y = y;
    a.dna.size = 10.0f;
    a.speciesId = std::uniform_int_distribution<int>(0, 4)(rng);
    a.energy = 100.0f;
    population.push_back(a);
}

void SimulationEngine::spawnFoodAt(float x, float y) {
    foodList.push_back({x, y, 30.0f, false});
}

void SimulationEngine::addZone(float x, float y, float radius, ZoneType type) {
    zones.push_back({x, y, radius, type});
}

void SimulationEngine::clearAllZones() {
    zones.clear();
}

// --- GRID LOGIC ---
void SimulationEngine::clearGrid() {
    for (int x = 0; x < GRID_COLS; ++x) {
        for (int y = 0; y < GRID_ROWS; ++y) {
            agentGrid[x][y].clear();
            foodGrid[x][y].clear();
        }
    }
}

void SimulationEngine::populateGrid() {
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

std::vector<Agent*> SimulationEngine::getAgentsInRadius(float x, float y, float radius) {
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

std::vector<Food*> SimulationEngine::getFoodInRadius(float x, float y, float radius) {
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

// --- GAME LOOP ---
void SimulationEngine::updateTick() {
    clearGrid();
    populateGrid();

    std::vector<Agent> newborns;

    for (size_t i = 0; i < population.size(); ++i) {
        auto& agent = population[i];
        if (!agent.isAlive) continue;

        // --- ENVIRONMENTAL ZONES ---
        float speedMultiplier = 1.0f;
        for (const auto& zone : zones) {
            if (getDistance(agent.x, agent.y, zone.x, zone.y) < zone.radius) {
                if (zone.type == ZoneType::POISON) {
                    agent.energy -= 0.5f * simSpeed; // Drain extra energy
                } else if (zone.type == ZoneType::WATER) {
                    speedMultiplier = 0.4f; // Slow down significantly
                }
            }
        }

        // Use the Grid to only get nearby objects!
        std::vector<Food*> nearbyFood = getFoodInRadius(agent.x, agent.y, agent.dna.vision);
        std::vector<Agent*> nearbyAgents = getAgentsInRadius(agent.x, agent.y, agent.dna.vision);

        // --- SENSES ---
        float nearestFoodDist = 9999.0f;
        Food* targetFood = nullptr;

        for (Food* food : nearbyFood) {
            if (food->isEaten) continue;
            float dist = getDistance(agent.x, agent.y, food->x, food->y);
            if (dist < nearestFoodDist) {
                nearestFoodDist = dist;
                targetFood = food;
            }
        }

        float nearestPreyDist = 9999.0f;
        float nearestPredatorDist = 9999.0f;
        Agent* targetPrey = nullptr;

        for (Agent* other : nearbyAgents) {
            if (&agent == other || !other->isAlive) continue;

            float dist = getDistance(agent.x, agent.y, other->x, other->y);
            if (agent.dna.size > other->dna.size * 1.2f) { // Prey
                if (dist < nearestPreyDist) {
                    nearestPreyDist = dist;
                    targetPrey = other;
                }
            } else if (other->dna.size > agent.dna.size * 1.2f) { // Predator
                if (dist < nearestPredatorDist) {
                    nearestPredatorDist = dist;
                }
            }
        }

        // Prepare Inputs: [dist_food, dist_prey, dist_predator, energy, age, density, random_noise]
        std::vector<float> inputs(NeuralNetwork::INPUT_NODES, 0.0f);
        inputs[0] = (nearestFoodDist < agent.dna.vision) ? (nearestFoodDist / agent.dna.vision) : 1.0f;
        inputs[1] = (nearestPreyDist < agent.dna.vision) ? (nearestPreyDist / agent.dna.vision) : 1.0f;
        inputs[2] = (nearestPredatorDist < agent.dna.vision) ? (nearestPredatorDist / agent.dna.vision) : 1.0f;
        inputs[3] = agent.energy / 200.0f;
        inputs[4] = (float)agent.age / agent.dna.lifespan;

        std::uniform_real_distribution<float> noiseDist(-1.0f, 1.0f);
        inputs[6] = noiseDist(rng);

        // --- BRAIN ---
        std::vector<float> outputs = agent.brain.feedForward(inputs);

        agent.velocityX = outputs[0] * agent.dna.speed * speedMultiplier;
        agent.velocityY = outputs[1] * agent.dna.speed * speedMultiplier;
        bool wantsToEat = outputs[2] > 0.0f;
        bool wantsToReproduce = outputs[3] > 0.5f;

        // --- PHYSICS ---
        agent.x += agent.velocityX * simSpeed;
        agent.y += agent.velocityY * simSpeed;

        if (agent.x < 0) agent.x += WORLD_WIDTH;
        if (agent.x > WORLD_WIDTH) agent.x -= WORLD_WIDTH;
        if (agent.y < 0) agent.y += WORLD_HEIGHT;
        if (agent.y > WORLD_HEIGHT) agent.y -= WORLD_HEIGHT;

        // --- METABOLISM ---
        float movementCost = std::sqrt(agent.velocityX*agent.velocityX + agent.velocityY*agent.velocityY);
        float sizeCost = agent.dna.size * 0.01f;
        agent.energy -= (agent.dna.metabolism + sizeCost + (movementCost * 0.05f)) * simSpeed;
        agent.age++;

        // --- EATING ---
        if (wantsToEat) {
            if (targetPrey != nullptr && nearestPreyDist < agent.dna.size) {
                agent.energy += targetPrey->energy * 0.8f;
                targetPrey->isAlive = false;
            } else if (targetFood != nullptr && nearestFoodDist < agent.dna.size) {
                agent.energy += targetFood->energyValue;
                targetFood->isEaten = true;
            }
        }

        // --- REPRODUCTION ---
        if (wantsToReproduce && agent.energy > agent.dna.fertility) {
            agent.energy -= (agent.dna.fertility * 0.5f);

            Agent child;
            child.x = agent.x + (noiseDist(rng) * 10.0f);
            child.y = agent.y + (noiseDist(rng) * 10.0f);
            child.energy = agent.dna.fertility * 0.4f;
            child.dna = agent.dna;
            child.speciesId = agent.speciesId;

            child.dna.size += noiseDist(rng) * 2.0f;
            if (child.dna.size < 3.0f) child.dna.size = 3.0f;

            child.dna.mutation_rate = globalMutationRate; // Apply global setting

            child.brain = agent.brain;
            child.brain.mutate(child.dna.mutation_rate, 0.5f);

            newborns.push_back(child);
        }

        if (agent.energy <= 0 || agent.age > agent.dna.lifespan) {
            agent.isAlive = false;
        }
    }

    // 2. Cleanup Dead Agents
    population.erase(
        std::remove_if(population.begin(), population.end(),
            [](const Agent& a) { return !a.isAlive; }),
        population.end()
    );

    // 3. Add Newborns
    population.insert(population.end(), newborns.begin(), newborns.end());

    // 4. Cleanup Food
    foodList.erase(
        std::remove_if(foodList.begin(), foodList.end(),
            [](const Food& f) { return f.isEaten; }),
        foodList.end()
    );

    if (foodList.size() < 150) {
        spawnFood(2);
    }
}
