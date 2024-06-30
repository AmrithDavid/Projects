package com.example.barometer

import android.util.Log
import java.io.InputStream
import java.io.BufferedReader
import java.io.InputStreamReader
import java.time.LocalDateTime
import java.time.format.DateTimeFormatter

class CsvParserService {
    private val inputFormatter = DateTimeFormatter.ofPattern("d/MM/yyyy H:mm:ss")
    private val outputFormatter = DateTimeFormatter.ofPattern("d/MM/yyyy HH:mm:ss")

    fun parseCsvFile(inputStream: InputStream): List<RocketData> {
        val reader = BufferedReader(InputStreamReader(inputStream))
        return reader.lineSequence()
            .drop(1) // Skip header
            .mapNotNull { line ->
                Log.d("CsvParserService", "Parsing line: $line")
                val parts = line.split(",")
                if (parts.size == 4) {
                    try {
                        val timestamp = LocalDateTime.parse(parts[0], inputFormatter)
                        RocketData(
                            timestamp = timestamp.format(outputFormatter),
                            altitude = parts[1].toDouble(),
                            pressure = parts[2].toDouble(),
                            temperature = parts[3].toDouble()
                        )
                    } catch (e: Exception) {
                        Log.e("CsvParserService", "Error parsing line: $line", e)
                        null
                    }
                } else {
                    Log.w("CsvParserService", "Invalid line format: $line")
                    null
                }
            }.toList()
    }
}