package com.algorithmx.evolutionsimulation.ui

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.unit.dp

@Composable
fun PopulationGraphOverlay(history: List<IntArray>, colors: List<Color>, modifier: Modifier = Modifier) {
    Canvas(modifier = modifier
        .fillMaxWidth()
        .height(150.dp)
        .padding(16.dp)
    ) {
        // Draw background panel
        drawRect(color = Color(0xAA000000), size = Size(size.width, size.height))

        // Find the highest population peak to scale the Y-axis
        val maxPop = history.maxOfOrNull { it.maxOrNull() ?: 0 }?.coerceAtLeast(10) ?: 10
        val stepX = size.width / 100f // 100 history points max

        // Draw a line for each of the 5 species
        for (speciesId in 0 until 5) {
            val path = Path()
            history.forEachIndexed { index, counts ->
                val x = index * stepX
                val y = size.height - ((counts[speciesId].toFloat() / maxPop) * size.height)
                
                if (index == 0) path.moveTo(x, y)
                else path.lineTo(x, y)
            }

            drawPath(
                path = path,
                color = colors[speciesId],
                style = Stroke(width = 3f)
            )
        }
    }
}
