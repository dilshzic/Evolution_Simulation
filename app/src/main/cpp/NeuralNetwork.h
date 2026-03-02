#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

/**
 * A lightweight, performant Feedforward Neural Network.
 * Architecture: 7 Inputs -> 5 Hidden -> 4 Outputs.
 */
class NeuralNetwork {
public:
    static constexpr int INPUT_NODES = 7;
    static constexpr int HIDDEN_NODES = 5;
    static constexpr int OUTPUT_NODES = 4;

    NeuralNetwork() {
        // Initialize weights and biases with random values
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

        // Layer 1 (Input to Hidden)
        weightsIH.resize(INPUT_NODES * HIDDEN_NODES);
        biasesH.resize(HIDDEN_NODES);
        for (float& w : weightsIH) w = dist(gen);
        for (float& b : biasesH) b = dist(gen);

        // Layer 2 (Hidden to Output)
        weightsHO.resize(HIDDEN_NODES * OUTPUT_NODES);
        biasesO.resize(OUTPUT_NODES);
        for (float& w : weightsHO) w = dist(gen);
        for (float& b : biasesO) b = dist(gen);
    }

    /**
     * Compute the outputs based on given inputs.
     * Inputs: [dist_food, dist_prey, dist_predator, energy, age, density, noise]
     * Outputs: [move_x, move_y, eat_attack, reproduce]
     */
    std::vector<float> feedForward(const std::vector<float>& inputs) const {
        std::vector<float> hidden(HIDDEN_NODES);

        // Input -> Hidden
        for (int h = 0; h < HIDDEN_NODES; ++h) {
            float sum = biasesH[h];
            for (int i = 0; i < INPUT_NODES; ++i) {
                sum += inputs[i] * weightsIH[h * INPUT_NODES + i];
            }
            hidden[h] = std::tanh(sum); // Activation
        }

        std::vector<float> outputs(OUTPUT_NODES);
        // Hidden -> Output
        for (int o = 0; o < OUTPUT_NODES; ++o) {
            float sum = biasesO[o];
            for (int h = 0; h < HIDDEN_NODES; ++h) {
                sum += hidden[h] * weightsHO[o * HIDDEN_NODES + h];
            }
            outputs[o] = std::tanh(sum); // Activation
        }

        return outputs;
    }

    /**
     * Randomly mutates weights and biases.
     */
    void mutate(float mutationRate, float mutationAmount) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
        std::uniform_real_distribution<float> amountDist(-mutationAmount, mutationAmount);

        auto mutateVec = [&](std::vector<float>& vec) {
            for (float& val : vec) {
                if (chanceDist(gen) < mutationRate) {
                    val += amountDist(gen);
                    // Clamp weights to keep network stable
                    val = std::clamp(val, -5.0f, 5.0f);
                }
            }
        };

        mutateVec(weightsIH);
        mutateVec(biasesH);
        mutateVec(weightsHO);
        mutateVec(biasesO);
    }

    /**
     * Returns a combined vector of all weights for visualization.
     */
    std::vector<float> getAllWeights() const {
        std::vector<float> combined = weightsIH;
        combined.insert(combined.end(), weightsHO.begin(), weightsHO.end());
        return combined;
    }

private:
    // Weights and biases stored in 1D vectors for cache-locality
    std::vector<float> weightsIH; // Hidden layer weights (size: 7 * 5)
    std::vector<float> biasesH;   // Hidden layer biases (size: 5)

    std::vector<float> weightsHO; // Output layer weights (size: 5 * 4)
    std::vector<float> biasesO;   // Output layer biases (size: 4)
};

#endif // NEURAL_NETWORK_H
