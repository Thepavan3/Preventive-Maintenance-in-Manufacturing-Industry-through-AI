/*
  This code reads temperature and pressure data from an Adafruit BMP085 sensor,
  displays the temperature on an I2C LCD display, and triggers actions based on
  temperature thresholds using Blynk.

  Make sure to install the required libraries before uploading this code.

  Library Dependencies:
  - Adafruit_Sensor: https://github.com/adafruit/Adafruit_Sensor
  - Adafruit_BMP085: https://github.com/adafruit/Adafruit_BMP085_Unified
  - LiquidCrystal_I2C: https://github.com/johnrickman/LiquidCrystal_I2C
  - BlynkSimpleEsp8266: https://github.com/blynkkk/blynk-library
  
  Blynk Setup:
  - Install the Blynk app on your smartphone.
  - Create a new project, obtain an Auth Token, and configure widgets.
  - Update BLYNK_TEMPLATE_ID, BLYNK_TEMPLATE_NAME, and BLYNK_AUTH_TOKEN with the appropriate values.
  - Update ssid and pass with your WiFi credentials.
*/

// Include required libraries
#include <ESP8266WiFi.h>              // Library for WiFi communication
#include <BlynkSimpleEsp8266.h>       // Library for Blynk integration
#include <Wire.h>                     // I2C communication library
#include <Adafruit_Sensor.h>          // Base class for sensor libraries
#include <Adafruit_BMP085.h>          // Library for BMP085 temperature and pressure sensor
#include <LiquidCrystal_I2C.h>        // Library for I2C LCD display

// Blynk template configuration
#define BLYNK_TEMPLATE_ID "TMPL2G5icuuBw"
#define BLYNK_TEMPLATE_NAME "temp"
#define BLYNK_AUTH_TOKEN "rt7QOlhifLDfoH92aohajof7b3xAxbHU"

#define BLYNK_PRINT Serial

// Pin configuration for the I2C communication with the LCD display
#define I2C_ADDR 0x27 // I2C address of the LCD display
#define LCD_COLS 16   // Number of columns in the LCD display
#define LCD_ROWS 2    // Number of rows in the LCD display
#define SDA_PIN D2    // SDA pin for I2C communication
#define SCL_PIN D1    // SCL pin for I2C communication
#define BUZZER_PIN D8 // Pin for the buzzer
#define RELAY_PIN D5  // Pin for the relay

// Initialize Blynk and WiFi settings
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "PAVAN";     // type your WiFi name
char pass[] = "9154683836"; // type your WiFi password

// Create objects for BMP085 sensor and LCD display
Adafruit_BMP085 bmp;
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);
BlynkTimer timer;

// Function to send sensor data to Blynk
void sendSensor() {
  // Read temperature and pressure from BMP085 sensor
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0;

  // Print temperature and pressure to Serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C    Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  // Send temperature and pressure data to Blynk
  Blynk.virtualWrite(V5, temperature); // V5 is the virtual pin for temperature
  Blynk.virtualWrite(V6, pressure);    // V6 is the virtual pin for pressure

  // Check if temperature exceeds a certain threshold
  if (temperature > 32) {
    // Trigger an event and log it in Blynk
    Blynk.logEvent("oil_quality", "Change the Oil!");
  }
}

void setup() {
  // Initialize Serial communication for debugging
  Serial.begin(115200);

  // Initialize Blynk with authentication and WiFi settings
  Blynk.begin(auth, ssid, pass);

  // Initialize I2C communication with SDA and SCL pins defined
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize BMP085 sensor
  bmp.begin();

  // Initialize LCD display
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Temperature Sensor");

  // Initialize buzzer and relay pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // Set interval for sensor reading and action execution using BlynkTimer
  timer.setInterval(2500L, sendSensor);
}

void loop() {
  // Run Blynk background tasks
  Blynk.run();

  // Run BlynkTimer tasks
  timer.run();

  // Read temperature from the BMP180 sensor
  float temperature = bmp.readTemperature();

  // Display the temperature on the LCD display
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  // Check if temperature exceeds a certain threshold
  if (temperature > 32.0) {
    // Activate the buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1000); // Buzzer duration (1 second)
    digitalWrite(BUZZER_PIN, LOW);

    // Cut off power supply using the relay
    digitalWrite(RELAY_PIN, HIGH); // Turn off the relay
  } else if (temperature < 40.0) {
    // Restore power supply using the relay
    digitalWrite(RELAY_PIN, LOW); // Turn on the relay
  }
}
