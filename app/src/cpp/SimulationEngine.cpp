#include "SimulationEngine.h"
#include <algorithm>

SimulationEngine::SimulationEngine() {
    std::random_device rd;
    rng = std::mt19937(rd());
    spawnFood(100);
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

void SimulationEngine::updateTick() {
    std::vector<Agent> newborns;

    // 1. Process all alive agents
    for (size_t i = 0; i < population.size(); ++i) {
        auto& agent = population[i];
        if (!agent.isAlive) continue;

        // --- SENSES (Inputs to Neural Network) ---
        float nearestFoodDist = 9999.0f;
        Food* targetFood = nullptr;

        // 1a. Scan for Plant Food
        for (auto& food : foodList) {
            if (food.isEaten) continue;
            float dist = getDistance(agent.x, agent.y, food.x, food.y);
            if (dist < nearestFoodDist && dist < agent.dna.vision) {
                nearestFoodDist = dist;
                targetFood = &food;
            }
        }

        // 1b. Scan for Predators and Prey (Other Agents)
        float nearestPreyDist = 9999.0f;
        float nearestPredatorDist = 9999.0f;
        Agent* targetPrey = nullptr;

        for (size_t j = 0; j < population.size(); ++j) {
            if (i == j || !population[j].isAlive) continue; // Don't look at yourself or the dead
            auto& other = population[j];

            float dist = getDistance(agent.x, agent.y, other.x, other.y);
            if (dist < agent.dna.vision) {
                // If the other agent is 20% smaller, we can eat it!
                if (agent.dna.size > other.dna.size * 1.2f) {
                    if (dist < nearestPreyDist) {
                        nearestPreyDist = dist;
                        targetPrey = &other;
                    }
                }
                // If the other agent is 20% bigger, run!
                else if (other.dna.size > agent.dna.size * 1.2f) {
                    if (dist < nearestPredatorDist) {
                        nearestPredatorDist = dist;
                    }
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

        // --- BRAIN (Feedforward) ---
        std::vector<float> outputs = agent.brain.feedForward(inputs);

        agent.velocityX = outputs[0] * agent.dna.speed;
        agent.velocityY = outputs[1] * agent.dna.speed;
        bool wantsToEat = outputs[2] > 0.0f;
        bool wantsToReproduce = outputs[3] > 0.5f;

        // --- PHYSICS & MOVEMENT ---
        agent.x += agent.velocityX;
        agent.y += agent.velocityY;

        if (agent.x < 0) agent.x += WORLD_WIDTH;
        if (agent.x > WORLD_WIDTH) agent.x -= WORLD_WIDTH;
        if (agent.y < 0) agent.y += WORLD_HEIGHT;
        if (agent.y > WORLD_HEIGHT) agent.y -= WORLD_HEIGHT;

        // --- METABOLISM ---
        float movementCost = std::sqrt(agent.velocityX*agent.velocityX + agent.velocityY*agent.velocityY);
        // Bigger size means higher base metabolism
        float sizeCost = agent.dna.size * 0.01f;
        agent.energy -= (agent.dna.metabolism + sizeCost + (movementCost * 0.05f));
        agent.age++;

        // --- EATING (Combat & Foraging) ---
        if (wantsToEat) {
            // Carnivores: If prey is in range, attack and eat!
            if (targetPrey != nullptr && nearestPreyDist < (agent.dna.size)) {
                agent.energy += targetPrey->energy * 0.8f; // Absorb 80% of the prey's energy
                targetPrey->isAlive = false; // Kill the prey
            }
            // Herbivores: Eat plant food
            else if (targetFood != nullptr && nearestFoodDist < agent.dna.size) {
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

            // Mutate DNA size slightly to allow predator/prey divergence
            child.dna.size += noiseDist(rng) * 2.0f;
            if (child.dna.size < 3.0f) child.dna.size = 3.0f; // Minimum size

            child.brain = agent.brain;
            child.brain.mutate(child.dna.mutation_rate, 0.5f);

            newborns.push_back(child);
        }

        // --- DEATH ---
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

    // 4. Cleanup Eaten Food & Respawn
    foodList.erase(
        std::remove_if(foodList.begin(), foodList.end(),
            [](const Food& f) { return f.isEaten; }),
        foodList.end()
    );

    if (foodList.size() < 100) {
        spawnFood(2);
    }
}
