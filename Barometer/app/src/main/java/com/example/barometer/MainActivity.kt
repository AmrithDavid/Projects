package com.example.barometer

import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.lifecycle.viewmodel.compose.viewModel
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowBack
import androidx.compose.ui.unit.dp
import androidx.compose.ui.text.style.TextOverflow

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            val viewModel: RocketViewModel = viewModel()
            val navController = rememberNavController()

            // Load data from CSV file in assets
            LaunchedEffect(Unit) {
                try {
                    assets.open("rocket_data.csv").use { inputStream ->
                        Log.d("MainActivity", "Loading CSV data")
                        viewModel.loadData(inputStream)
                    }
                } catch (e: Exception) {
                    Log.e("MainActivity", "Error loading CSV: ${e.message}", e)
                }
            }

            NavHost(navController = navController, startDestination = "home") {
                composable("home") {
                    HomeScreen(
                        onNavigateToData = { navController.navigate("data") },
                        onNavigateToSettings = { navController.navigate("settings") }
                    )
                }
                composable("data") {
                    DataScreen(
                        viewModel = viewModel,
                        onNavigateBack = { navController.navigateUp() }
                    )
                }
                composable("settings") {
                    SettingsScreen(
                        viewModel = viewModel,
                        onNavigateBack = { navController.navigateUp() }
                    )
                }
            }
        }
    }
}

@Composable
fun HomeScreen(onNavigateToData: () -> Unit, onNavigateToSettings: () -> Unit) {
    Column(modifier = Modifier.fillMaxSize(), verticalArrangement = Arrangement.Center) {
        Button(onClick = onNavigateToData) {
            Text("View Rocket Data")
        }
        Button(onClick = onNavigateToSettings) {
            Text("Settings")
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun DataScreen(viewModel: RocketViewModel, onNavigateBack: () -> Unit) {
    val rocketData by viewModel.rocketData.collectAsState()
    val showAltitude by viewModel.showAltitude.collectAsState()
    val showPressure by viewModel.showPressure.collectAsState()
    val showTemperature by viewModel.showTemperature.collectAsState()

    Log.d("DataScreen", "Received ${rocketData.size} items")

    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text("Rocket Data") },
                navigationIcon = {
                    IconButton(onClick = onNavigateBack) {
                        Icon(Icons.Filled.ArrowBack, contentDescription = "Back")
                    }
                }
            )
        }
    ) { innerPadding ->
        LazyColumn(
            modifier = Modifier
                .fillMaxSize()
                .padding(innerPadding)
        ) {
            items(rocketData) { data ->
                Log.d("DataScreen", "Displaying item: $data")
                Card(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(horizontal = 16.dp, vertical = 8.dp),
                    elevation = CardDefaults.cardElevation(defaultElevation = 4.dp)
                ) {
                    Column(modifier = Modifier.padding(16.dp)) {
                        Text(
                            "Timestamp: ${data.timestamp}",
                            style = MaterialTheme.typography.bodyLarge,
                            maxLines = 1,
                            overflow = TextOverflow.Ellipsis
                        )
                        Spacer(modifier = Modifier.height(4.dp))
                        if (showAltitude) Text("Altitude: ${data.altitude} m", style = MaterialTheme.typography.bodyMedium)
                        if (showPressure) Text("Pressure: ${data.pressure} hPa", style = MaterialTheme.typography.bodyMedium)
                        if (showTemperature) Text("Temperature: ${data.temperature} °C", style = MaterialTheme.typography.bodyMedium)
                    }
                }
            }
        }
    }
}

@Composable
fun SettingsScreen(viewModel: RocketViewModel, onNavigateBack: () -> Unit) {
    val showAltitude by viewModel.showAltitude.collectAsState()
    val showPressure by viewModel.showPressure.collectAsState()
    val showTemperature by viewModel.showTemperature.collectAsState()

    Column {
        Button(onClick = onNavigateBack) {
            Text("Back")
        }
        Switch(
            checked = showAltitude,
            onCheckedChange = { viewModel.toggleAltitude() }
        )
        Text("Show Altitude")
        Switch(
            checked = showPressure,
            onCheckedChange = { viewModel.togglePressure() }
        )
        Text("Show Pressure")
        Switch(
            checked = showTemperature,
            onCheckedChange = { viewModel.toggleTemperature() }
        )
        Text("Show Temperature")
    }
}