#include <jni.h>
#include <string>
#include <vector>
#include "SimulationEngine.h"

// Static instance of the simulation engine
static SimulationEngine g_engine;

extern "C" JNIEXPORT jstring JNICALL
Java_com_algorithmx_evolutionsimulation_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jfloatArray JNICALL
Java_com_algorithmx_evolutionsimulation_MainActivity_getAgentData(
        JNIEnv* env,
        jobject /* this */) {

    // For demonstration, let's add some dummy agents if the population is empty
    if (g_engine.getPopulation().empty()) {
        for (int i = 0; i < 100; ++i) {
            Agent a;
            a.x = static_cast<float>(rand() % 1000);
            a.y = static_cast<float>(rand() % 1000);
            a.dna.size = 5.0f + static_cast<float>(rand() % 10);
            a.speciesId = rand() % 5;
            g_engine.getPopulation().push_back(a);
        }
    }

    // Update the simulation state
    g_engine.updateTick();

    const std::vector<Agent>& agents = g_engine.getPopulation();

    // Filter alive agents and collect their data
    std::vector<float> packedData;
    packedData.reserve(agents.size() * 4);

    for (const auto& agent : agents) {
        if (agent.isAlive) {
            packedData.push_back(agent.x);
            packedData.push_back(agent.y);
            packedData.push_back(agent.dna.size);
            packedData.push_back(static_cast<float>(agent.speciesId));
        }
    }

    // Create and populate the jfloatArray
    jfloatArray result = env->NewFloatArray(static_cast<jsize>(packedData.size()));
    if (result == nullptr) {
        return nullptr; // Out of memory
    }
    env->SetFloatArrayRegion(result, 0, static_cast<jsize>(packedData.size()), packedData.data());

    return result;
}
