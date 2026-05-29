#include <ESP32Servo.h>

#define WIPER_RELAY_PIN 27
#define HEATER_RELAY_PIN 25
#define SERVO_PIN 26

Servo wiperServo;

bool wiperActive = false;
int servoPos = 30;
int direction = 1;

const int MIN_ANGLE = 0;
const int MAX_ANGLE = 120;

// 🔥 Increased speed settings
const int SPEED_DELAY = 4;   // smaller delay = faster
const int STEP_SIZE = 8;     // bigger step = faster movement

unsigned long lastMove = 0;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);

  digitalWrite(WIPER_RELAY_PIN, HIGH);
  digitalWrite(HEATER_RELAY_PIN, HIGH);
  pinMode(WIPER_RELAY_PIN, OUTPUT);
  pinMode(HEATER_RELAY_PIN, OUTPUT);

  wiperServo.attach(SERVO_PIN);
  wiperServo.write(MIN_ANGLE);

  Serial.println("Actuator Listening for UWB Alerts...");
}

void loop() {

  if (Serial2.available()) {
    String incoming = Serial2.readString();
    incoming.trim();

    Serial.print("RAW DATA: ");
    Serial.println(incoming);

    if (incoming.indexOf("RAIN_FOG") != -1) {
      controlHardware(true, true);
    } 
    else if (incoming.indexOf("RAIN") != -1) {
      controlHardware(true, false);
    } 
    else if (incoming.indexOf("FOG") != -1) {
      controlHardware(false, true);
    } 
    else if (incoming.indexOf("CLEAR") != -1) {
      controlHardware(false, false);
    }
  }

  if (wiperActive) {
    sweepWiper();
  }
}

void controlHardware(bool wiper, bool heater) {

  if (wiper) {
    digitalWrite(WIPER_RELAY_PIN, LOW);
    wiperActive = true;
  } 
  else {
    digitalWrite(WIPER_RELAY_PIN, HIGH);
    wiperActive = false;
    servoPos = MIN_ANGLE;
    wiperServo.write(MIN_ANGLE);
  }

  digitalWrite(HEATER_RELAY_PIN, heater ? LOW : HIGH);
}

void sweepWiper() {

  if (millis() - lastMove > SPEED_DELAY) {

    servoPos += direction * STEP_SIZE;

    if (servoPos >= MAX_ANGLE) {
      servoPos = MAX_ANGLE;
      direction = -1;
    }

    if (servoPos <= MIN_ANGLE) {
      servoPos = MIN_ANGLE;
      direction = 1;
      delay(80);   // shorter pause for faster repeat
    }

    wiperServo.write(servoPos);
    lastMove = millis();
  }
}