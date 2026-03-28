package com.algorithmx.evolutionsimulation

object NativeEngine {
    init {
        System.loadLibrary("evolutionsimulation")
    }

    external fun getAgentData(): FloatArray
    external fun getAgentBrain(x: Float, y: Float): FloatArray
    external fun spawnAgentNative(x: Float, y: Float)
    external fun spawnFoodNative(x: Float, y: Float)
    external fun setMutationRateNative(rate: Float)
    external fun setSimSpeedNative(speed: Float)
    external fun getZoneData(): FloatArray
    external fun getFoodData(): FloatArray
}
