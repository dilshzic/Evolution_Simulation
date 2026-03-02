package com.algorithmx.evolutionsimulation

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.drawscope.Fill
import androidx.compose.ui.platform.LocalContext

class MainActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            SimulationScreen(this)
        }
    }

    /**
     * Retrieves agent data from the native C++ engine.
     * Returns a flattened array where each agent is [x, y, size, speciesId].
     */
    external fun getAgentData(): FloatArray

    companion object {
        init {
            System.loadLibrary("evolutionsimulation")
        }
    }
}

@Composable
fun SimulationScreen(activity: MainActivity) {
    // We use a state to hold the agent data.
    var agentData by remember { mutableStateOf(floatArrayOf()) }

    // Simulation loop using LaunchedEffect and withFrameNanos for smooth updates
    // This runs on the composition's CoroutineContext, synced with the display refresh rate.
    LaunchedEffect(Unit) {
        while (true) {
            withFrameNanos {
                agentData = activity.getAgentData()
            }
        }
    }

    Canvas(modifier = Modifier.fillMaxSize()) {
        val data = agentData
        if (data.isEmpty()) return@Canvas

        val agentCount = data.size / 4
        
        for (i in 0 until agentCount) {
            val offset = i * 4
            val x = data[offset]
            val y = data[offset + 1]
            val size = data[offset + 2]
            val speciesId = data[offset + 3].toInt()

            // Choose color based on speciesId
            val color = when (speciesId % 5) {
                0 -> Color(0xFFE57373) // Red
                1 -> Color(0xFF81C784) // Green
                2 -> Color(0xFF64B5F6) // Blue
                3 -> Color(0xFFFFF176) // Yellow
                else -> Color(0xFFBA68C8) // Purple
            }

            drawCircle(
                color = color,
                radius = size,
                center = Offset(x, y),
                style = Fill
            )
        }
    }
}
