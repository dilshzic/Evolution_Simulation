package com.algorithmx.evolutionsimulation

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.gestures.detectTapGestures
import androidx.compose.foundation.layout.*
import androidx.compose.material3.Slider
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.drawscope.Fill
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.unit.dp
import com.algorithmx.evolutionsimulation.ui.BrainVisualizerOverlay
import com.algorithmx.evolutionsimulation.ui.PopulationGraphOverlay
import kotlin.math.abs

class MainActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            SimulationScreen(this)
        }
    }

    external fun getAgentData(): FloatArray
    external fun getAgentBrain(x: Float, y: Float): FloatArray
    external fun spawnAgentNative(x: Float, y: Float)
    external fun spawnFoodNative(x: Float, y: Float)
    external fun setMutationRateNative(rate: Float)
    external fun setSimSpeedNative(speed: Float)
    external fun getZoneData(): FloatArray
    external fun getFoodData(): FloatArray

    companion object {
        init {
            System.loadLibrary("evolutionsimulation")
        }
    }
}

@Composable
fun SimulationScreen(activity: MainActivity) {
    var agentData by remember { mutableStateOf(floatArrayOf()) }
    var zoneData by remember { mutableStateOf(floatArrayOf()) }
    var foodData by remember { mutableStateOf(floatArrayOf()) }
    var selectedBrain by remember { mutableStateOf(floatArrayOf()) }
    var tapPosition by remember { mutableStateOf<Offset?>(null) }
    
    var mutationRate by remember { mutableFloatStateOf(0.05f) }
    var simSpeed by remember { mutableFloatStateOf(1.0f) }

    var populationHistory by remember { mutableStateOf(listOf<IntArray>()) }
    var frameCounter by remember { mutableStateOf(0) }

    val speciesColors = remember {
        listOf(
            Color(0xFFE57373), Color(0xFF81C784), Color(0xFF64B5F6), Color(0xFFFFF176), Color(0xFFBA68C8)
        )
    }

    LaunchedEffect(Unit) {
        while (true) {
            withFrameNanos {
                agentData = activity.getAgentData()
                zoneData = activity.getZoneData()
                foodData = activity.getFoodData()
                
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
                    val brain = activity.getAgentBrain(pos.x, pos.y)
                    if (brain.isNotEmpty()) selectedBrain = brain
                }
            }
        }
    }

    Box(modifier = Modifier.fillMaxSize()) {
        Canvas(modifier = Modifier
            .fillMaxSize()
            .pointerInput(Unit) {
                detectTapGestures(
                    onTap = { offset ->
                        tapPosition = offset
                        val brain = activity.getAgentBrain(offset.x, offset.y)
                        if (brain.isEmpty()) {
                            tapPosition = null
                            selectedBrain = floatArrayOf()
                        }
                    },
                    onLongPress = { offset ->
                        activity.spawnAgentNative(offset.x, offset.y)
                    }
                )
            }
        ) {
            // Draw Zones
            for (i in 0 until zoneData.size / 4) {
                val zx = zoneData[i * 4]
                val zy = zoneData[i * 4 + 1]
                val zr = zoneData[i * 4 + 2]
                val zt = zoneData[i * 4 + 3].toInt()
                val zColor = if (zt == 0) Color(0x44FF0000) else Color(0x440000FF)
                drawCircle(color = zColor, radius = zr, center = Offset(zx, zy))
            }

            // Draw Food
            for (i in 0 until foodData.size / 2) {
                drawCircle(color = Color.Green, radius = 4f, center = Offset(foodData[i * 2], foodData[i * 2 + 1]))
            }

            // Draw Agents
            val data = agentData
            val agentCount = data.size / 4
            for (i in 0 until agentCount) {
                val offset = i * 4
                val x = data[offset]
                val y = data[offset + 1]
                val size = data[offset + 2]
                val speciesId = (data[offset + 3].toInt() % 5 + 5) % 5
                val color = speciesColors[speciesId]
                drawCircle(color = color, radius = size, center = Offset(x, y), style = Fill)
                
                tapPosition?.let { currentTap ->
                    if (abs(currentTap.x - x) < 50 && abs(currentTap.y - y) < 50) {
                        drawCircle(color = Color.White, radius = size + 5f, center = Offset(x, y), style = Stroke(width = 3f))
                    }
                }
            }
        }

        // Control Panel
        Column(
            modifier = Modifier
                .align(Alignment.TopEnd)
                .padding(16.dp)
                .background(Color(0x88000000))
                .padding(8.dp)
                .width(200.dp)
        ) {
            Text("Mutation Rate: ${"%.2f".format(mutationRate)}", color = Color.White)
            Slider(value = mutationRate, onValueChange = { mutationRate = it; activity.setMutationRateNative(it) }, valueRange = 0f..0.5f)
            Text("Sim Speed: ${"%.1f".format(simSpeed)}x", color = Color.White)
            Slider(value = simSpeed, onValueChange = { simSpeed = it; activity.setSimSpeedNative(it) }, valueRange = 0.1f..5f)
        }

        if (selectedBrain.isNotEmpty()) BrainVisualizerOverlay(brainData = selectedBrain)
        if (populationHistory.isNotEmpty()) {
            PopulationGraphOverlay(history = populationHistory, colors = speciesColors, modifier = Modifier.align(Alignment.BottomCenter))
        }
    }
}
