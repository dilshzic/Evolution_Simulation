#include <jni.h>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include "SimulationEngine.h"

static SimulationEngine g_engine;

extern "C" JNIEXPORT jfloatArray JNICALL
Java_com_algorithmx_evolutionsimulation_MainActivity_getAgentData(
        JNIEnv* env,
        jobject /* this */) {

    if (g_engine.getPopulation().empty()) {
        for (int i = 0; i < 50; ++i) {
            g_engine.spawnAgentAt(static_cast<float>(rand() % 1080), static_cast<float>(rand() % 2000));
        }
        // Add some initial zones
        g_engine.addZone(300.0f, 500.0f, 200.0f, ZoneType::WATER);
        g_engine.addZone(800.0f, 1500.0f, 150.0f, ZoneType::POISON);
    }

    g_engine.updateTick();

    const std::vector<Agent>& agents = g_engine.getPopulation();
    std::vector<float> packedData;
    packedData.reserve(agents.size() * 4);

    for (const auto& agent : agents) {
        packedData.push_back(agent.x);
        packedData.push_back(agent.y);
        packedData.push_back(agent.dna.size);
        packedData.push_back(static_cast<float>(agent.speciesId));
    }

    jfloatArray result = env->NewFloatArray(static_cast<jsize>(packedData.size()));
    if (result == nullptr) return nullptr;
    env->SetFloatArrayRegion(result, 0, static_cast<jsize>(packedData.size()), packedData.data());

    return result;
}

extern "C" JNIEXPORT jfloatArray JNICALL
Java_com_algorithmx_evolutionsimulation_MainActivity_getAgentBrain(
        JNIEnv* env,
        jobject /* this */,
        jfloat touchX,
        jfloat touchY) {

    const std::vector<Agent>& agents = g_engine.getPopulation();
    const Agent* closestAgent = nullptr;
    float minSourceDist = 100.0f;

    for (const auto& agent : agents) {
        float dx = agent.x - touchX;
        float dy = agent.y - touchY;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < minSourceDist) {
            minSourceDist = dist;
            closestAgent = &agent;
        }
    }

    if (closestAgent != nullptr) {
        std::vector<float> weights = closestAgent->brain.getAllWeights();
        jfloatArray result = env->NewFloatArray(static_cast<jsize>(weights.size()));
        if (result != nullptr) {
            env->SetFloatArrayRegion(result, 0, static_cast<jsize>(weights.size()), weights.data());
            return result;
        }
    }

    return env->NewFloatArray(0);
}

extern "C" JNIEXPORT void JNICALL
Java_com_algorithmx_evolutionsimulation_MainActivity_spawnAgentNative(
        JNIEnv* env,
        jobject /* this */,
        jfloat x,
        jfloat y) {
    g_engine.spawnAgentAt(x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_algorithmx_evolutionsimulation_MainActivity_spawnFoodNative(
        JNIEnv* env,
        jobject /* this */,
        jfloat x,
        jfloat y) {
    g_engine.spawnFoodAt(x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_algorithmx_evolutionsimulation_MainActivity_setMutationRateNative(
        JNIEnv* env,
        jobject /* this */,
        jfloat rate) {
    g_engine.setMutationRate(rate);
}

extern "C" JNIEXPORT void JNICALL
Java_com_algorithmx_evolutionsimulation_MainActivity_setSimSpeedNative(
        JNIEnv* env,
        jobject /* this */,
        jfloat speed) {
    g_engine.setSimSpeed(speed);
}

extern "C" JNIEXPORT jfloatArray JNICALL
Java_com_algorithmx_evolutionsimulation_MainActivity_getZoneData(
        JNIEnv* env,
        jobject /* this */) {

    const std::vector<Zone>& zones = g_engine.getZones();
    std::vector<float> packedData;
    packedData.reserve(zones.size() * 4);

    for (const auto& zone : zones) {
        packedData.push_back(zone.x);
        packedData.push_back(zone.y);
        packedData.push_back(zone.radius);
        packedData.push_back(static_cast<float>(zone.type));
    }

    jfloatArray result = env->NewFloatArray(static_cast<jsize>(packedData.size()));
    if (result == nullptr) return nullptr;
    env->SetFloatArrayRegion(result, 0, static_cast<jsize>(packedData.size()), packedData.data());

    return result;
}

extern "C" JNIEXPORT jfloatArray JNICALL
Java_com_algorithmx_evolutionsimulation_MainActivity_getFoodData(
        JNIEnv* env,
        jobject /* this */) {

    const std::vector<Food>& food = g_engine.getFood();
    std::vector<float> packedData;
    packedData.reserve(food.size() * 2);

    for (const auto& f : food) {
        if (!f.isEaten) {
            packedData.push_back(f.x);
            packedData.push_back(f.y);
        }
    }

    jfloatArray result = env->NewFloatArray(static_cast<jsize>(packedData.size()));
    if (result == nullptr) return nullptr;
    env->SetFloatArrayRegion(result, 0, static_cast<jsize>(packedData.size()), packedData.data());

    return result;
}
