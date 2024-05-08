#include <Adafruit_BMP280.h> // Include Adafruit BMP280 library for barometric pressure sensor
#include <Arduino.h> // Include Arduino standard library
// #include <Buzzer.h> // Optional include for a buzzer component (currently commented out)
#include <SPIMemory.h> // Include SPIMemory library for SPI flash memory interaction
#include <tones.h> // Include tones library for generating sound tones

// Constants for serial communication and hardware pins (commented out)
// #define BAUD_RATE 9600
// #define FLASH_CS 10
// #define BEEPER_PIN 5

SPIFlash* flash; // Pointer to a SPIFlash object to interact with flash memory
Adafruit_BMP280* barometer; // Pointer to a BMP280 object for the barometric sensor

// Global variables for rocket telemetry data
int _addr;
unsigned long numRecords;
float padAltitude = 0.0;
float padPressure = 0.0;

// A class to filter and average altitude measurements
class Filter {
 private:
  static const int avgSize = 50; // Size of the rolling average array
  int avgIndex = 0; // Current index in the rolling average array
  float rollingAvg[avgSize] = {0}; // Array to store rolling averages

  // Function to compute the average altitude from the rolling average array
  float getAltitude() {
    float avg = 0;
    for (int i = 0; i < avgSize; i++) {
      avg += rollingAvg[i];
    }
    avg /= (float)avgSize;
    return avg;
  }

 public:
  // Constructor and destructor
  Filter(){};
  ~Filter(){};

  // Function to update the altitude, add it to the rolling array and return the averaged value
  float updateAltitude(float rawAlt) {
    rollingAvg[avgIndex++] = rawAlt;
    avgIndex %= avgSize;
    return getAltitude();
  }

} *filter; // Instance of the Filter class

// Enumeration for different states of the rocket
enum LaunchState { onPad, launched, landed };

// Function to print the current launch state to the serial port
void printLaunchState(const LaunchState& state) {
  switch (state) {
    case LaunchState::onPad:
      Serial.print("On Pad");
      break;
    case LaunchState::launched:
      Serial.print("Launched");
      break;
    case LaunchState::landed:
      Serial.print("Landed");
      break;
  }
}

// Structure to store the state of the rocket
struct RocketState {
  unsigned long timestamp;
  LaunchState launchState;
  float altitude;
  float rawAltitude;
  float roll;
  float pitch;
  float yaw;
};

// Function to print the current state of the rocket to the serial port
void printRocketState(const RocketState& state) {
  Serial.print("[");
  Serial.print(state.timestamp);
  Serial.print(", ");
  printLaunchState(state.launchState);
  Serial.print("] altitude: ");
  Serial.print(state.altitude);
  Serial.print(", raw altitude: ");
  Serial.print(state.rawAltitude);
  Serial.print(", roll: ");
  Serial.print(state.roll);
  Serial.print(", pitch: ");
  Serial.print(state.pitch);
  Serial.print(", yaw: ");
  Serial.print(state.yaw);
  Serial.println();
}

namespace Global {
  // Constants for launch and land detection thresholds
  const float LAUNCH_CUTOFF = 2.0;
  const float LAND_CUTOFF = 1.0;
  LaunchState launch_state; // Variable to store current launch state
  RocketState rocket_state; // Variable to store current rocket state
  
}  // namespace Global
// void dumpData() {
//   Serial.begin(BAUD_RATE);
//   while (!Serial)
//     ;

//   flash->readAnything(0, numRecords);
//   _addr = sizeof(numRecords);

//   for (int i = 0; i < numRecords and digitalRead(PB_PIN) == HIGH; i++) {
//     flash->readAnything(_addr, Global::rocket_state);
//     _addr += sizeof(Global::rocket_state);
//     printRocketState(Global::rocket_state);
//   }
// }
void printFlashDetails() {
  uint32_t JEDEC = flash->getJEDECID();
  if (!JEDEC) {
    Serial.println("No comms. Check wiring. Is chip supported?");
  } else {
    // Print flash memory details
    Serial.print("JEDEC ID: 0x");
    Serial.println(JEDEC, HEX);
    Serial.print("Man ID: 0x");
    Serial.println(uint8_t(JEDEC >> 16), HEX);
    Serial.print("Memory ID: 0x");
    Serial.println(uint8_t(JEDEC >> 8), HEX);
    Serial.print("Capacity: ");
    Serial.println(flash->getCapacity());
    Serial.print("Max Pages: ");
    Serial.println(flash->getMaxPage());
  }
  long long _uniqueID = flash->getUniqueID();
  if (_uniqueID) {
    // Print ID of the flash memory
    Serial.print("Unique ID: ");
    Serial.print(uint32_t(_uniqueID / 1000000L));
    Serial.print(uint32_t(_uniqueID % 1000000L));
    Serial.print(", ");
    Serial.print("0x");
    Serial.print(uint32_t(_uniqueID >> 32), HEX);
    Serial.println(uint32_t(_uniqueID), HEX);
  }
}

// Function to initialize the flash memory
bool initialiseFlash() {
  flash = new SPIFlash(FLASH_CS); // Initialize a new SPIFlash object
  flash->begin(); // Begin communication with the flash memory
  uint32_t JEDEC = flash->getJEDECID();
  if (!JEDEC) {
    Serial.println("No comms. Check wiring. Is chip supported?");
    return false;
  }
  return true;
}

// Function to initialize the altimeter (BMP280 sensor)
bool initialiseAltimeter() {
  barometer = new Adafruit_BMP280(); // Create a new BMP280 object
  unsigned status;
  status = barometer->begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
  // Check if the sensor initialization was successful
  if (!status) {
    Serial.println(
        F("Could not find a valid BMP280 sensor, check wiring or "
          "try a different address!"));
    Serial.print("SensorID was: 0x");
    Serial.println(barometer->sensorID(), 16);
    Serial.print(
        "        ID of 0xFF probably means a bad address, a BMP 180 or BMP "
        "085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    return false;
  }
  // Default settings from datasheet for BMP280
  barometer->setSampling(
      Adafruit_BMP280::MODE_NORMAL,     
      Adafruit_BMP280::SAMPLING_X2,     
      Adafruit_BMP280::SAMPLING_X2,     
      Adafruit_BMP280::FILTER_X16,      
      Adafruit_BMP280::STANDBY_MS_500); 

  // Read initial pressure to set the pad pressure
  int numAvgs = 5;
  for (int i = 0; i < numAvgs; i++) {
    padPressure += barometer->readPressure();
    delay(10);
  }
  padPressure /= numAvgs;
  padPressure /= 100;  // Convert Pa to hPa
  padAltitude = barometer->readAltitude(padPressure); // Calculate pad altitude
  return true;
}

// Arduino setup function
void setup() {
  pinMode(BEEPER_PIN, OUTPUT); // Set the beeper pin as an output
  Serial.begin(BAUD_RATE); // Begin serial communication at defined baud rate

  filter = new Filter(); // Create a new Filter object

  bool goForLaunch = true;

  // Initialize the altimeter and check if it was successful
  if (goForLaunch &= initialiseAltimeter()) {
    Serial.println("\nBarometer initialised.");
    Serial.print("Pad Pressure: ");
    Serial.print(padPressure);
    Serial.println(" hPa");
  }

  // Initialize the flash memory and check if it was successful
  if (goForLaunch &= initialiseFlash()) {
    Serial.println("\nFlash initialised.");
    printFlashDetails(); // Print details of the flash memory
  }

  // If initialization failed, sound a failure tone and halt
 if (!goForLaunch) {
    Serial.println("\nFailed to initialise.");
    // Play a sequence of tones to indicate failure
    tone(BEEPER_PIN, NOTE_C4, 300);
    delay(300);
    tone(BEEPER_PIN, NOTE_AS3, 300);
    delay(300);
    tone(BEEPER_PIN, NOTE_F3, 300);
    delay(300);

    while (true); // Infinite loop to halt further execution
  }

  Serial.println("\nGo for launch!");
  // Play a sequence of tones to indicate readiness for launch
  tone(BEEPER_PIN, NOTE_C4, 300);
  delay(200);
  tone(BEEPER_PIN, NOTE_E4, 300);
  delay(200);
  tone(BEEPER_PIN, NOTE_A4, 300);
  delay(200);

  Global::rocket_state.launchState = LaunchState::onPad; // Set initial launch state
  _addr = sizeof(numRecords); // Initialize the address for data storage
  numRecords = -1; // Initialize the number of records
}

long lastMillis = 0; // Variable to store the last time the loop ran

void loop() {
  // Check if one second has passed
  if (millis() - lastMillis > 1000) {
    printRocketState(Global::rocket_state); // Print the current rocket state
    lastMillis = millis(); // Update the last time the loop ran
  }

  // Switch case to handle different launch states
  switch (Global::rocket_state.launchState) {
    case LaunchState::onPad: {
      // Read and update rocket state while on pad
      Global::rocket_state.timestamp = millis(); // Update timestamp
      Global::rocket_state.rawAltitude = barometer->readAltitude(padPressure); // Read raw altitude
      Global::rocket_state.altitude = filter->updateAltitude(Global::rocket_state.rawAltitude); // Update filtered altitude

      // Check if the rocket has launched
      if (Global::rocket_state.altitude > Global::LAUNCH_CUTOFF) {
        Global::rocket_state.launchState = LaunchState::launched;
        Serial.println("[STATE] Rocket Launched!");
        numRecords = 0; // Reset the number of records after launch
      }
      break;
    }

    case LaunchState::launched: {
      // Update rocket state during launch
      Global::rocket_state.timestamp = millis();
      Global::rocket_state.rawAltitude = barometer->readAltitude(padPressure);
      Global::rocket_state.altitude = filter->updateAltitude(Global::rocket_state.rawAltitude);

      
      // Global::rocket_state.roll = imu->getRoll();
      // Global::rocket_state.pitch = imu->getPitch();
      // Global::rocket_state.yaw = imu->getYaw();

      
      // flash->writeAnything(_addr, Global::rocket_state);
      // _addr += sizeof(Global::rocket_state);

      // Check if the rocket has landed
      if (Global::rocket_state.altitude < padAltitude + Global::LAND_CUTOFF) {
        Global::rocket_state.launchState = LaunchState::landed;
        Serial.println("[STATE] Rocket Landed!");
      }
      break;
    }

    case LaunchState::landed: {
      // Handle post-landing operations
      break;

    default:
      // Safety fallback in case of an undefined state
      if (numRecords == (unsigned long)-1) {
        flash->writeAnything(0, numRecords);
      }
      while (true); // Infinite loop to prevent any further actions
      break;
  }
  }

  delay(50); // Short delay to reduce loop execution frequency
}
