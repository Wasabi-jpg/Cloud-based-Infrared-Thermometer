#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <TFT_eSPI.h> //For OLED

#define FAN_PIN 32
#define BUTTON_PIN 33
#define TEMPERATURE_THRESHOLD 25.0
//MAC Address: A0:DD:6C:6F:32:40

bool fan_on = false;             // Tracks the fan's state

TFT_eSPI tft = TFT_eSPI(); //using the OLED


void tempReceive(const uint8_t *macAddr, const uint8_t *data, int dataLen) {
    // Print the sender's MAC address
  Serial.print("Data received from MAC address: ");
  for (int i = 0; i < 6; i++) {
    if (macAddr[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(macAddr[i], HEX);
    if (i < 5) {
      Serial.print(":");
    }
  }
  Serial.println();
  //----------------------- Temp data ----------------------
    Serial.println("tempReceive called");
    if (dataLen > 0) {
        // Convert received data to string and parse temperature
        char tempStr[dataLen + 1];
        memcpy(tempStr, data, dataLen);
        tempStr[dataLen] = '\0';  // Null-terminate the string

        float temperature = atof(tempStr);  // Convert string to float

        // Print the received temperature
        Serial.print("Received Temperature: ");
        Serial.println(temperature);

        // display temperature touched on OLED
        tft.fillScreen(TFT_BLACK);  // Clear the screen
        tft.setTextSize(2);
        tft.setCursor(10, 10);
        tft.print("Temp: ");
        tft.print(temperature);  // display the temperature

        // If the temperature exceeds the threshold, turn the fan on
        if (temperature >= TEMPERATURE_THRESHOLD) {
            if (!fan_on) {
                digitalWrite(FAN_PIN, HIGH);  // Turn on the fan
                fan_on = true;
                Serial.println("Fan turned ON");
            }
        } else {
            if (fan_on) {
                digitalWrite(FAN_PIN, LOW);  // Turn off the fan
                fan_on = false;
                Serial.println("Fan turned OFF");
            }
        }
    }
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA); // Set the ESP32 to station mode
  if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
  }
  pinMode(FAN_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(FAN_PIN, LOW);
  esp_now_register_recv_cb(tempReceive);
  Serial.println("Ready to activate fan");
  Serial.println("Mac address:");
  Serial.println(WiFi.macAddress());


  // seting up OLED display
  tft.init();
  tft.setRotation(1);

   // display starting info on OLED
    tft.fillScreen(TFT_BLACK);  // Clear the screen
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print("Mac Addr: ");
    tft.print(WiFi.macAddress());  // display the temperature
}

void loop() {
  // digitalWrite(FAN_PIN, HIGH); // Force fan ON
  // delay(5000);
  // digitalWrite(FAN_PIN, LOW); // Force fan OFF
  // delay(5000);
}
