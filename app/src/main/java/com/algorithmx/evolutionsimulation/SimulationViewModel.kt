package com.algorithmx.evolutionsimulation

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.compose.runtime.withFrameNanos
import androidx.compose.ui.geometry.Offset
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.launch

class SimulationViewModel : ViewModel() {
    var agentData by mutableStateOf(floatArrayOf())
        private set
    var zoneData by mutableStateOf(floatArrayOf())
        private set
    var foodData by mutableStateOf(floatArrayOf())
        private set
    var selectedBrain by mutableStateOf(floatArrayOf())
        private set
    var tapPosition by mutableStateOf<Offset?>(null)
    
    var mutationRate by mutableFloatStateOf(0.05f)
        private set
    var simSpeed by mutableFloatStateOf(1.0f)
        private set

    var populationHistory by mutableStateOf(listOf<IntArray>())
        private set
    private var frameCounter = 0

    init {
        viewModelScope.launch {
            while (true) {
                withFrameNanos {
                    agentData = NativeEngine.getAgentData()
                    zoneData = NativeEngine.getZoneData()
                    foodData = NativeEngine.getFoodData()
                    
                    frameCounter++
                    if (frameCounter % 10 == 0) {
                        val currentCounts = IntArray(5) { 0 }
                        val agentCount = agentData.size / 4
                        for (i in 0 until agentCount) {
                            val speciesId = (agentData[i * 4 + 3].toInt() % 5 + 5) % 5
                            currentCounts[speciesId]++
                        }
                        populationHistory = (populationHistory + currentCounts).takeLast(100)
                    }

                    tapPosition?.let { pos ->
                        val brain = NativeEngine.getAgentBrain(pos.x, pos.y)
                        selectedBrain = brain
                    }
                }
            }
        }
    }

    fun onTap(offset: Offset) {
        tapPosition = offset
        val brain = NativeEngine.getAgentBrain(offset.x, offset.y)
        if (brain.isEmpty()) {
            tapPosition = null
            selectedBrain = floatArrayOf()
        }
    }

    fun onDoubleTap(offset: Offset) {
        NativeEngine.spawnFoodNative(offset.x, offset.y)
    }

    fun onLongPress(offset: Offset) {
        NativeEngine.spawnAgentNative(offset.x, offset.y)
    }

    fun updateMutationRate(rate: Float) {
        mutationRate = rate
        NativeEngine.setMutationRateNative(rate)
    }

    fun updateSimSpeed(speed: Float) {
        simSpeed = speed
        NativeEngine.setSimSpeedNative(speed)
    }
}
