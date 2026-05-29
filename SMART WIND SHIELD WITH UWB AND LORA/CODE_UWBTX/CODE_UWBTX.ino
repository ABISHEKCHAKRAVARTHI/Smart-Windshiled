#include "DHT.h"

// Pin Definitions based on your ESP32 Pinout
#define DHTPIN 15
#define DHTTYPE DHT22
#define RAIN_PIN 34

// Thresholds - Adjusted to 30.0 as requested
float TEMP_THRESHOLD = 30.0; 
int RAIN_THRESHOLD = 2500;   // Lower value means more water detected

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);        // Debug to Computer
  
  // RYLR998 LoRa on Serial2 (GPIO 16=RX, 17=TX)
  Serial2.begin(115200, SERIAL_8N1, 16, 17); 
  
  // RYUW122 UWB on Serial1 (GPIO 13=RX, 14=TX) 
  // Note: TX on ESP32 goes to RX on Module
  Serial1.begin(115200, SERIAL_8N1, 13, 14); 

  dht.begin();
  pinMode(RAIN_PIN, INPUT);

  Serial.println("--- Brain Node Initialized ---");
}

void loop() {
  float temp = dht.readTemperature();
  int rainVal = analogRead(RAIN_PIN);
  
  // Check if sensor reading failed
  if (isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    temp = 0; 
  }

  String status = "CLEAR";

  // Logic for detection
  bool isRaining = (rainVal < RAIN_THRESHOLD);
  bool isFoggy = (temp > TEMP_THRESHOLD); 

  if (isRaining && isFoggy) status = "RAIN_FOG";
  else if (isRaining) status = "RAIN";
  else if (isFoggy) status = "FOG";
  else status = "CLEAR";

  // --- 1. SEND VIA UWB (Internal Actuator) ---
  // Command: AT+ANCHOR_SEND=<Address>,<PayloadLength>,<Data>
  String uwbCmd = "AT+ANCHOR_SEND=SLAVE001," + String(status.length()) + "," + status + "\r\n";
  Serial1.print(uwbCmd);
  Serial1.flush(); // Ensures data is fully sent before the loop continues

  // --- 2. SEND VIA LORA (Rear Vehicle Car-2) ---
  // Command: AT+SEND=<Address>,<PayloadLength>,<Data>
  String loraCmd = "AT+SEND=2," + String(status.length()) + "," + status + "\r\n";
  Serial2.print(loraCmd);
  Serial2.flush();

  // Local Debugging
  Serial.print("Temp: "); Serial.print(temp);
  Serial.print("°C | Rain Value: "); Serial.print(rainVal);
  Serial.print(" | Sending Status: "); Serial.println(status);

  delay(2000); // 2-second interval to avoid flooding the modules
}