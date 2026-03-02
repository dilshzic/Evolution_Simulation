#ifndef AGENT_H
#define AGENT_H

#include "NeuralNetwork.h"

enum class ZoneType {
    POISON, // Drains energy
    WATER   // Slows movement
};

struct Zone {
    float x;
    float y;
    float radius;
    ZoneType type;
};

struct DNA {
    float speed = 2.0f;
    float size = 10.0f;
    float vision = 50.0f;
    float aggression = 0.5f;
    float metabolism = 0.1f;
    float fertility = 150.0f; // Energy required to reproduce
    float lifespan = 1000.0f;
    float camouflage = 0.0f;
    float memory = 0.0f;
    float neural_bias = 0.0f;
    float mutation_rate = 0.05f;
};

struct Agent {
    DNA dna;
    NeuralNetwork brain;

    float x = 0.0f;
    float y = 0.0f;
    float velocityX = 0.0f;
    float velocityY = 0.0f;
    float energy = 100.0f;
    int age = 0;
    bool isAlive = true;
    int speciesId = 0;

    Agent() = default;
};

struct Food {
    float x;
    float y;
    float energyValue = 30.0f;
    bool isEaten = false;
};

#endif // AGENT_H
