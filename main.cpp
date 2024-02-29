#include <Adafruit_BMP280.h>
#include <Arduino.h>
#include <SPIMemory.h>

#define BAUD_RATE 9600
#define FLASH_CS 10
#define BEEPER_PIN 5 // Uncomment and adjust pin number as necessary

SPIFlash flash(FLASH_CS); // Create SPIFlash object with Chip Select pin
Adafruit_BMP280 barometer; // Create Adafruit_BMP280 object

float padAltitude = 0.0;
float padPressure = 0.0;

// Filter class to smooth the altitude readings
class Filter {
private:
  static const int avgSize = 50;
  int avgIndex = 0;
  float rollingAvg[avgSize] = {0};

  float getAltitude() {
    float avg = 0;
    for (int i = 0; i < avgSize; i++) {
      avg += rollingAvg[i];
    }
    avg /= avgSize;
    return avg;
  }

public:
  Filter() {}
  
  float updateAltitude(float rawAlt) {
    rollingAvg[avgIndex++] = rawAlt;
    if (avgIndex >= avgSize) avgIndex = 0;
    return getAltitude();
  }
};

Filter filter; // Instantiate a Filter object

enum LaunchState { onPad, launched, landed };

LaunchState launchState = onPad; // Initial launch state

void setup() {
  Serial.begin(BAUD_RATE);
  while (!Serial); // Wait for serial port to connect

  if (!barometer.begin()) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  // Default settings from datasheet
  barometer.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X2, Adafruit_BMP280::SAMPLING_X2, Adafruit_BMP280::FILTER_X16, Adafruit_BMP280::STANDBY_MS_500);

  // Initialize pad pressure and altitude
  int numReadings = 5;
  for (int i = 0; i < numReadings; i++) {
    padPressure += barometer.readPressure();
    delay(10); // Short delay between readings
  }
  padPressure /= numReadings; // Average pressure
  padPressure /= 100.0; // Convert to hPa
  padAltitude = barometer.readAltitude(padPressure); // Calculate initial altitude
  
  Serial.print("Pad Pressure: ");
  Serial.print(padPressure);
  Serial.println(" hPa");
  Serial.print("Pad Altitude: ");
  Serial.print(padAltitude);
  Serial.println(" m");
  
  if (!flash.begin()) {
    Serial.println("Flash memory initialization failed!");
    while (1);
  }
}

void loop() {
  float currentAltitude = barometer.readAltitude(padPressure); // Read current altitude
  currentAltitude = filter.updateAltitude(currentAltitude); // Filter the altitude reading

  // Check launch state transitions
  switch (launchState) {
    case onPad:
      if (currentAltitude - padAltitude > 2.0) { // Arbitrary launch threshold
        launchState = launched;
        Serial.println("Launch detected!");
      }
      break;
    case launched:
      if (currentAltitude - padAltitude < 1.0) { // Arbitrary landing threshold
        launchState = landed;
        Serial.println("Landing detected!");
      }
      break;
    case landed:
      // Optional: Implement landing logic or reset
      break;
  }

  // Optional: Implement data logging to flash memory here

  delay(1000); // Delay before next loop iteration
}
