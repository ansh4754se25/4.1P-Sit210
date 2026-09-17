#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

// Pin connections
const int pirPin = 3;
const int buttonPin = 2;

const int led1 = 4;
const int led2 = 5;

// Light level below 50 lux is considered dark
const float darknessThreshold = 50.0;

// Interrupt flags
volatile bool motionDetected = false;
volatile bool buttonPressed = false;

// Current LED state
bool lightsOn = false;

// Used for button debounce
unsigned long lastButtonTime = 0;

// PIR interrupt
void motionInterrupt() {
  motionDetected = true;
}

// Button interrupt
void buttonInterrupt() {
  buttonPressed = true;
}

// Turn both LEDs ON
void turnLightsOn() {
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  lightsOn = true;
}

// Turn both LEDs OFF
void turnLightsOff() {
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  lightsOn = false;
}

void setup() {
  // Start Serial Monitor
  Serial.begin(9600);

  // Configure LEDs
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  // Start with lights OFF
  turnLightsOff();

  // Configure PIR sensor
  pinMode(pirPin, INPUT);

  // Configure button using internal pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP);

  // Start BH1750
  Wire.begin();

  if (lightMeter.begin()) {
    Serial.println("BH1750 started");
  } else {
    Serial.println("BH1750 not detected");
  }

  // Enable PIR interrupt
  attachInterrupt(
    digitalPinToInterrupt(pirPin),
    motionInterrupt,
    RISING
  );

  // Enable button interrupt
  attachInterrupt(
    digitalPinToInterrupt(buttonPin),
    buttonInterrupt,
    FALLING
  );

  Serial.println("System Started");
  Serial.println("Waiting for motion or button...");
}

void loop() {
  // Read the current light level
  float lightLevel = lightMeter.readLightLevel();

  // Handle PIR interrupt
  if (motionDetected) {
    motionDetected = false;

    Serial.println("Motion detected");

    // Automatically turn lights ON only when it is dark
    if (lightLevel < darknessThreshold) {
      turnLightsOn();

      Serial.print("It is dark: ");
      Serial.print(lightLevel);
      Serial.println(" lux");

      Serial.println("Lights ON");
    } else {
      Serial.print("It is bright: ");
      Serial.print(lightLevel);
      Serial.println(" lux");

      Serial.println("Lights remain OFF");
    }
  }

  // Handle button interrupt
  if (buttonPressed) {
    buttonPressed = false;

    // Wait for button bouncing to settle
    if (millis() - lastButtonTime > 200) {

      // Make sure the button is actually pressed
      if (digitalRead(buttonPin) == LOW) {

        // Toggle both LEDs
        if (lightsOn) {
          turnLightsOff();
          Serial.println("Button pressed - Lights OFF");
        } else {
          turnLightsOn();
          Serial.println("Button pressed - Lights ON");
        }

        lastButtonTime = millis();
      }
    }
  }
}
