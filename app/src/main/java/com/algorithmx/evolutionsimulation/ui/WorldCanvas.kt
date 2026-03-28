package com.algorithmx.evolutionsimulation.ui

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.gestures.detectTapGestures
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.drawscope.Fill
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.input.pointer.pointerInput
import com.algorithmx.evolutionsimulation.SimulationViewModel
import kotlin.math.abs

@Composable
fun WorldCanvas(
    viewModel: SimulationViewModel,
    speciesColors: List<Color>
) {
    Canvas(modifier = Modifier
        .fillMaxSize()
        .pointerInput(Unit) {
            detectTapGestures(
                onTap = { offset -> viewModel.onTap(offset) },
                onDoubleTap = { offset -> viewModel.onDoubleTap(offset) },
                onLongPress = { offset -> viewModel.onLongPress(offset) }
            )
        }
    ) {
        // Draw Zones
        val zoneData = viewModel.zoneData
        for (i in 0 until zoneData.size / 4) {
            val zx = zoneData[i * 4]
            val zy = zoneData[i * 4 + 1]
            val zr = zoneData[i * 4 + 2]
            val zt = zoneData[i * 4 + 3].toInt()
            val zColor = if (zt == 0) Color(0x44FF0000) else Color(0x440000FF)
            drawCircle(color = zColor, radius = zr, center = Offset(zx, zy))
        }

        // Draw Food
        val foodData = viewModel.foodData
        for (i in 0 until foodData.size / 2) {
            drawCircle(color = Color.Green, radius = 4f, center = Offset(foodData[i * 2], foodData[i * 2 + 1]))
        }

        // Draw Agents
        val agentData = viewModel.agentData
        val agentCount = agentData.size / 4
        for (i in 0 until agentCount) {
            val offset = i * 4
            val x = agentData[offset]
            val y = agentData[offset + 1]
            val size = agentData[offset + 2]
            val speciesId = (agentData[offset + 3].toInt() % 5 + 5) % 5
            val color = speciesColors[speciesId]
            drawCircle(color = color, radius = size, center = Offset(x, y), style = Fill)
            
            viewModel.tapPosition?.let { currentTap ->
                if (abs(currentTap.x - x) < 50 && abs(currentTap.y - y) < 50) {
                    drawCircle(color = Color.White, radius = size + 5f, center = Offset(x, y), style = Stroke(width = 3f))
                }
            }
        }
    }
}
