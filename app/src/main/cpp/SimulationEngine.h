#ifndef SIMULATION_ENGINE_H
#define SIMULATION_ENGINE_H

#include <vector>
#include <cstdint>
#include "NeuralNetwork.h"

/**
 * DNA represents the genetic traits of an Agent.
 * These values are typically used to determine behavior and physical characteristics.
 */
struct DNA {
    float speed = 0.0f;
    float size = 0.0f;
    float vision = 0.0f;
    float aggression = 0.0f;
    float metabolism = 0.0f;
    float fertility = 0.0f;
    float lifespan = 0.0f;
    float camouflage = 0.0f;
    float memory = 0.0f;
    float neural_bias = 0.0f;
    float mutation_rate = 0.0f;
};

/**
 * Agent represents an individual entity in the simulation.
 */
struct Agent {
    DNA dna;
    NeuralNetwork brain;

    // Current state variables
    float x = 0.0f;
    float y = 0.0f;
    float energy = 100.0f;
    int age = 0;
    bool isAlive = true;
    int speciesId = 0;

    Agent() = default;
};

/**
 * SimulationEngine manages the population and the simulation loop.
 */
class SimulationEngine {
public:
    SimulationEngine() = default;

    /**
     * updateTick is the core simulation loop where logic for movement,
     * energy depletion, and neural network calculations will reside.
     */
    void updateTick() {
        // TODO: Implement movement, energy depletion, and neural network calculations.
    }

    // Accessors for the population
    [[nodiscard]] const std::vector<Agent>& getPopulation() const { return population; }
    [[nodiscard]] std::vector<Agent>& getPopulation() { return population; }

private:
    std::vector<Agent> population;
};

#endif // SIMULATION_ENGINE_H
