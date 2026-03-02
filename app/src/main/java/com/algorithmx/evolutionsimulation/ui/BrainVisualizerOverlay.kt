package com.algorithmx.evolutionsimulation.ui

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.drawscope.Fill
import androidx.compose.ui.unit.dp
import kotlin.math.abs

@Composable
fun BrainVisualizerOverlay(brainData: FloatArray) {
    Canvas(modifier = Modifier
        .padding(16.dp)
        .size(width = 200.dp, height = 250.dp)
    ) {
        drawRect(color = Color(0xAA000000), size = Size(size.width, size.height))

        val inputNodes = 7
        val hiddenNodes = 5
        val outputNodes = 4

        val col1X = size.width * 0.15f
        val col2X = size.width * 0.5f
        val col3X = size.width * 0.85f

        fun getNodeY(index: Int, total: Int): Float = (size.height / (total + 1)) * (index + 1)

        if (brainData.size >= 35) {
            var weightIndex = 0
            for (h in 0 until hiddenNodes) {
                val hY = getNodeY(h, hiddenNodes)
                for (i in 0 until inputNodes) {
                    val iY = getNodeY(i, inputNodes)
                    val weight = brainData[weightIndex++]
                    val color = if (weight > 0) Color.Green else Color.Red
                    drawLine(color = color.copy(alpha = (abs(weight) / 5f).coerceIn(0f, 1f)), start = Offset(col1X, iY), end = Offset(col2X, hY), strokeWidth = abs(weight) * 2f)
                }
            }
        }

        if (brainData.size >= 55) {
            var weightIndex = 35
            for (o in 0 until outputNodes) {
                val oY = getNodeY(o, outputNodes)
                for (h in 0 until hiddenNodes) {
                    val hY = getNodeY(h, hiddenNodes)
                    val weight = brainData[weightIndex++]
                    val color = if (weight > 0) Color.Green else Color.Red
                    drawLine(color = color.copy(alpha = (abs(weight) / 5f).coerceIn(0f, 1f)), start = Offset(col2X, hY), end = Offset(col3X, oY), strokeWidth = abs(weight) * 2f)
                }
            }
        }

        for (i in 0 until inputNodes) drawCircle(Color.White, 8f, Offset(col1X, getNodeY(i, inputNodes)))
        for (h in 0 until hiddenNodes) drawCircle(Color.Gray, 8f, Offset(col2X, getNodeY(h, hiddenNodes)))
        for (o in 0 until outputNodes) drawCircle(Color.Cyan, 8f, Offset(col3X, getNodeY(o, outputNodes)))
    }
}
