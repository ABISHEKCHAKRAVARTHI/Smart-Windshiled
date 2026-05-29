#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    for(;;);
  }
  
  showBootScreen();
}

void loop() {
  if (Serial2.available()) {
    String incoming = Serial2.readStringUntil('\n');
    
    display.clearDisplay();
    drawHeader(); // Keeps a professional "V2V SYSTEM" bar at the top

    if (incoming.indexOf("FAST") >= 0) {
      drawAlertFrame("!! DANGER !!", "EXTREME RAIN");
    } 
    else if (incoming.indexOf("RAIN_FOG") >= 0) {
      drawAlertFrame("WARNING", "VISIBILITY LOW");
    } 
    else if (incoming.indexOf("RAIN") >= 0) {
      drawAlertFrame("CAUTION", "ROAD IS WET");
    } 
    else if (incoming.indexOf("FOG") >= 0) {
      drawAlertFrame("CAUTION", "DENSE FOG");
    } 
    else {
      drawNormalFrame("STATUS OK", "ROAD CLEAR");
    }
    
    display.display();
  }
}

// Adds a professional inverted top bar
void drawHeader() {
  display.fillRect(0, 0, 128, 14, WHITE);
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setCursor(35, 3);
  display.print("V2V ACTIVE");
}

// Design for dangerous/urgent alerts
void drawAlertFrame(String title, String msg) {
  display.setTextColor(WHITE);
  display.drawRect(0, 18, 128, 46, WHITE); // Border
  
  display.setTextSize(2);
  int16_t x1, y1; uint16_t w, h;
  display.getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((128 - w) / 2, 25);
  display.println(title);
  
  display.setTextSize(1);
  display.getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((128 - w) / 2, 48);
  display.println(msg);
}

// Design for normal/safe status
void drawNormalFrame(String title, String msg) {
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(38, 25);
  display.println(title);
  
  display.setTextSize(2);
  int16_t x1, y1; uint16_t w, h;
  display.getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((128 - w) / 2, 42);
  display.println(msg);
}

void showBootScreen() {
  display.clearDisplay();
  display.drawRoundRect(10, 10, 108, 44, 8, WHITE);
  display.setTextSize(2);
  display.setCursor(25, 25);
  display.println("SMART");
  display.display();
  delay(1000);
}