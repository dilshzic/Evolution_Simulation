package com.algorithmx.evolutionsimulation.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.material3.Slider
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import com.algorithmx.evolutionsimulation.SimulationViewModel

@Composable
fun SimulationScreen(viewModel: SimulationViewModel) {
    val speciesColors = remember {
        listOf(
            Color(0xFFE57373), Color(0xFF81C784), Color(0xFF64B5F6), Color(0xFFFFF176), Color(0xFFBA68C8)
        )
    }

    Box(modifier = Modifier.fillMaxSize()) {
        WorldCanvas(viewModel = viewModel, speciesColors = speciesColors)

        // Control Panel
        Column(
            modifier = Modifier
                .align(Alignment.TopEnd)
                .padding(16.dp)
                .background(Color(0x88000000))
                .padding(8.dp)
                .width(200.dp)
        ) {
            Text("Mutation Rate: ${"%.2f".format(viewModel.mutationRate)}", color = Color.White)
            Slider(
                value = viewModel.mutationRate,
                onValueChange = { viewModel.updateMutationRate(it) },
                valueRange = 0f..0.5f
            )
            Text("Sim Speed: ${"%.1f".format(viewModel.simSpeed)}x", color = Color.White)
            Slider(
                value = viewModel.simSpeed,
                onValueChange = { viewModel.updateSimSpeed(it) },
                valueRange = 0.1f..5f
            )
        }

        if (viewModel.selectedBrain.isNotEmpty()) {
            BrainVisualizerOverlay(brainData = viewModel.selectedBrain)
        }
        
        if (viewModel.populationHistory.isNotEmpty()) {
            PopulationGraphOverlay(
                history = viewModel.populationHistory,
                colors = speciesColors,
                modifier = Modifier.align(Alignment.BottomCenter)
            )
        }
    }
}
