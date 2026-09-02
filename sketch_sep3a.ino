#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

// Pin connections
const int pirPin = 2;
const int buttonPin = 5;

const int led1 = 3;
const int led2 = 4;

// Darkness threshold
const float darknessThreshold = 50.0;

// Automatic light duration
const unsigned long lightDuration = 10000;

// PIR interrupt flag
volatile bool motionDetected = false;

// Light status
bool lightsOn = false;

// Button mode
bool manualMode = false;

unsigned long lastLightTime = 0;


// PIR Interrupt
void motionInterrupt() {
  motionDetected = true;
}


// Turn both lights ON
void turnLightsOn(String reason) {

  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);

  lightsOn = true;

  Serial.println(reason);
}


// Turn both lights OFF
void turnLightsOff(String reason) {

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);

  lightsOn = false;

  Serial.println(reason);
}


void setup() {

  Serial.begin(9600);

  // LED pins
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  // PIR sensor
  pinMode(pirPin, INPUT);

  // Push button
  pinMode(buttonPin, INPUT_PULLUP);

  // Start BH1750
  Wire.begin();
  lightMeter.begin();

  // Attach PIR interrupt
  attachInterrupt(
    digitalPinToInterrupt(pirPin),
    motionInterrupt,
    RISING
  );

  Serial.println("System Started");
}


void loop() {

  // Read light level
  float lightLevel = lightMeter.readLightLevel();


  // -------------------------
  // PIR Automatic System
  // -------------------------
  if (motionDetected) {

    motionDetected = false;

    // Only activate if it is dark
    if (lightLevel < darknessThreshold && !manualMode) {

      turnLightsOn("Motion detected in darkness - Lights ON");

      // Start automatic timer
      lastLightTime = millis();

    } else if (lightLevel >= darknessThreshold) {

      Serial.println("Motion detected but it is bright");
    }
  }


  // -------------------------
  // Manual Push Button
  // -------------------------
  if (digitalRead(buttonPin) == LOW) {

    // Toggle manual mode
    manualMode = !manualMode;

    if (manualMode) {

      turnLightsOn("Button pressed - Lights ON");

    } else {

      turnLightsOff("Button pressed - Lights OFF");
    }


    // Wait until button is released
    while (digitalRead(buttonPin) == LOW) {
      delay(10);
    }

    delay(50); // Debounce
  }


  // -------------------------
  // Automatic Timer
  // -------------------------
  if (lightsOn &&
      !manualMode &&
      millis() - lastLightTime >= lightDuration) {

    turnLightsOff("Automatic timer - Lights OFF");
  }
}