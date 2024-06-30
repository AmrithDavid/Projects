package com.example.barometer

import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch
import java.io.InputStream

class RocketViewModel : ViewModel() {
    private val _rocketData = MutableStateFlow<List<RocketData>>(emptyList())
    val rocketData = _rocketData.asStateFlow()

    private val _showAltitude = MutableStateFlow(true)
    val showAltitude = _showAltitude.asStateFlow()

    private val _showPressure = MutableStateFlow(true)
    val showPressure = _showPressure.asStateFlow()

    private val _showTemperature = MutableStateFlow(true)
    val showTemperature = _showTemperature.asStateFlow()

    fun loadData(inputStream: InputStream) {
        viewModelScope.launch {
            val parsedData = CsvParserService().parseCsvFile(inputStream)
            Log.d("RocketViewModel", "Loaded ${parsedData.size} items")
            _rocketData.value = parsedData
        }
    }

    fun toggleAltitude() {
        _showAltitude.value = !_showAltitude.value
    }

    fun togglePressure() {
        _showPressure.value = !_showPressure.value
    }

    fun toggleTemperature() {
        _showTemperature.value = !_showTemperature.value
    }
}