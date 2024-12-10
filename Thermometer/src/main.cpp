#include <Arduino.h>
#include <HttpClient.h>
#include <WiFi.h>
#include <inttypes.h>
#include <stdio.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MLX90614.h>
#include <esp_now.h>
#include <TFT_eSPI.h> //For OLED

#include <esp_now.h>
#include <WiFi.h>

#define BUTTON_PIN 26
#define FAN_PIN 32
#define TEMPERATURE_THRESHOLD 25.0
uint8_t fanMAC[] = {0xA0, 0xDD, 0x6C, 0x6F, 0x32, 0x40}; // Replace with fan TTGO's MAC address
float currentTemperature = 0.0;

TFT_eSPI tft = TFT_eSPI(); //using the OLED

// This example downloads the URL "http://arduino.cc/"

char ssid[] = ""; // your network SSID (name)
char pass[] = ""; // your network password (use for WPA, or use
                            // as key for WEP)

// Name of the server we want to connect to
const char kHostname[] = "worldtimeapi.org";
// Path to download (this is the bit after the hostname in the URL
// that you want to download
const char kPath[] = "/api/timezone/Europe/London.txt";

// MLX90614 Sensor Configuration
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Server details
const char serverIP[] = "52.53.158.242"; // Replace with your server IP
const int serverPort = 5000;

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30 * 1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;


void setup() {
    Serial.begin(9600);

    //--------ESP-Now code--------------------
    WiFi.mode(WIFI_STA);
    Serial.println("MAC address: ");
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, fanMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    esp_err_t result = esp_now_add_peer(&peerInfo);
    if (result != ESP_OK) {
        Serial.print("Failed to add peer: ");
        Serial.println(esp_err_to_name(result));
    } else {
        Serial.println("Peer added successfully!");
    }

    //-------------------------------------
    //--------------Setting Up Button-----------
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    pinMode(FAN_PIN, OUTPUT);
    digitalWrite(FAN_PIN, LOW);

    // Initialize MLX90614 sensor
    if (!mlx.begin())
    {
      Serial.println("Error initializing MLX90614 sensor. Check wiring!");
      while (true)
        ; // Halt execution
    }
    Serial.println("MLX90614 sensor initialized");
// -----------------------------------------------
// --------- WiFi Setup -----------------------
  // Connect to WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());
//---------------------------------------------
   // seting up OLED display
  tft.init();
  tft.setRotation(1);

}

void loop() {
    char tempStr[10];
    snprintf(tempStr, sizeof(tempStr), "%f", currentTemperature);  // Convert temperature to string
    esp_now_send(fanMAC, (uint8_t *)tempStr, sizeof(tempStr)); // ESP-NOW communication

      if (digitalRead(BUTTON_PIN) == LOW)
      { // Button pressed (active LOW)
        Serial.println("Button pressed! Taking temperature reading...");
        // Read object and ambient temperatures
        float ambientTemp = mlx.readAmbientTempC();
        float objectTemp = mlx.readObjectTempC();

        currentTemperature = objectTemp;
        // char tempStr[10];
        // snprintf(tempStr, sizeof(tempStr), "%f", objectTemp);  // Convert temperature to string
        // esp_now_send(fanMAC, (uint8_t *)tempStr, sizeof(tempStr)); // ESP-NOW communication

        //--------------------------- OLED------------------------------------

          // display temperature touched on OLED
        tft.fillScreen(TFT_BLACK);  // Clear the screen
        tft.setTextSize(2);
        tft.setCursor(10, 10);
        tft.print("Temp: ");
        tft.print(objectTemp);  // display the temperature
        //--------------------------------------------------------------------

        // Check if the readings are valid
        if (isnan(ambientTemp) || isnan(objectTemp))
        {
          Serial.println("Failed to read from MLX90614 sensor!");
          delay(2000);
          return;
        }

        // Print sensor values for debugging
        Serial.print("Ambient Temperature: ");
        Serial.print(ambientTemp);
        Serial.print(" °C, Object Temperature: ");
        Serial.print(objectTemp);
        Serial.println(" °C");

        // Prepare the JSON payload
        char jsonPayload[128];
        snprintf(jsonPayload, sizeof(jsonPayload), "{\"ambientTemp\": %.2f, \"objectTemp\": %.2f}", ambientTemp, objectTemp);

        // Open a connection to the server
        WiFiClient client;
        if (!client.connect(serverIP, serverPort))
        {
          Serial.println("Connection failed");
          delay(2000);
          return;
        }

        // Construct the HTTP POST request
        String request = String("POST /data HTTP/1.1\r\n") +
                        "Host: " + String(serverIP) + "\r\n" +
                        "Content-Type: application/json\r\n" +
                        "Content-Length: " + String(strlen(jsonPayload)) + "\r\n" +
                        "Connection: close\r\n\r\n" +
                        jsonPayload;

        // Send the HTTP POST request
        client.print(request);
        Serial.println("Data sent to server: ");
        Serial.println(request);

        // Read the server's response (for debugging)
        while (client.connected() || client.available())
        {
          if (client.available())
          {
            String response = client.readString();
            Serial.println("Server response:");
            Serial.println(response);
          }
        }

        // Close the connection
        client.stop();
      }
      if(currentTemperature >= TEMPERATURE_THRESHOLD){
        digitalWrite(FAN_PIN, HIGH);
      }else{
        digitalWrite(FAN_PIN, LOW);
      }
      // And just stop, now that we've tried a download
      // while (1)
      //   ;
      delay(100);
}

//------- True code: -------------------------------------------------------------------------------------------
// #define BUTTON_PIN 26
// #define FAN_PIN 32
// #define TEMPERATURE_THRESHOLD 25.0
// uint8_t fanMAC[] = {0xA0, 0xDD, 0x6C, 0x6F, 0x32, 0x40}; // Replace with fan TTGO's MAC address
// // float temperatureThreshold = 30.0; // Example threshold
// float currentTemperature = 0.0;

// TFT_eSPI tft = TFT_eSPI(); //using the OLED

// // This example downloads the URL "http://arduino.cc/"

// char ssid[] = "S24";//"slayer3000"; // your network SSID (name)
// char pass[] = "Y33THay!";//"password";   // your network password (use for WPA, or use
//                             // as key for WEP)

// // Name of the server we want to connect to
// const char kHostname[] = "worldtimeapi.org";
// // Path to download (this is the bit after the hostname in the URL
// // that you want to download
// const char kPath[] = "/api/timezone/Europe/London.txt";

// // MLX90614 Sensor Configuration
// Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// // Server details
// const char serverIP[] = "52.53.158.242"; // Replace with your server IP
// const int serverPort = 5000;

// // Number of milliseconds to wait without receiving any data before we give up
// const int kNetworkTimeout = 30 * 1000;
// // Number of milliseconds to wait if no data is available before trying again
// const int kNetworkDelay = 1000;

// void setup()
// {
//   // Initialize Serial Monitor
//   Serial.begin(9600);
//   // WiFi.mode(WIFI_STA);
//   // if (esp_now_init() != ESP_OK) {
//   //       Serial.println("Error initializing ESP-NOW");
//   //       return;
//   // }
//   // esp_now_peer_info_t peerInfo = {};
//   // memcpy(peerInfo.peer_addr, fanMAC, 6);
//   // peerInfo.channel = 0;  
//   // peerInfo.encrypt = false;
//   // esp_now_add_peer(&peerInfo);

//   delay(1000);

//   // Setting Up Button
//   pinMode(BUTTON_PIN, INPUT_PULLUP);

//   pinMode(FAN_PIN, OUTPUT);
//   digitalWrite(FAN_PIN, LOW);

//   // Initialize MLX90614 sensor
//   if (!mlx.begin())
//   {
//     Serial.println("Error initializing MLX90614 sensor. Check wiring!");
//     while (true)
//       ; // Halt execution
//   }
//   Serial.println("MLX90614 sensor initialized");

//   // Connect to WiFi
//   Serial.println();
//   Serial.print("Connecting to ");
//   Serial.println(ssid);
//   WiFi.begin(ssid, pass);
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWiFi connected");
//   Serial.println("IP address: ");
//   Serial.println(WiFi.localIP());
//   Serial.println("MAC address: ");
//   Serial.println(WiFi.macAddress());

//    // seting up OLED display
//   tft.init();
//   tft.setRotation(1);
// }

// void loop()
// {
//   if (digitalRead(BUTTON_PIN) == LOW)
//   { // Button pressed (active LOW)
//     Serial.println("Button pressed! Taking temperature reading...");
//     // Read object and ambient temperatures
//     float ambientTemp = mlx.readAmbientTempC();
//     float objectTemp = mlx.readObjectTempC();

//     currentTemperature = objectTemp;
//     // char tempStr[10];
//     // snprintf(tempStr, sizeof(tempStr), "%f", objectTemp);  // Convert temperature to string
//     // esp_now_send(fanMAC, (uint8_t *)tempStr, sizeof(tempStr)); // ESP-NOW communication

//     //--------------------------- OLED------------------------------------

//       // display temperature touched on OLED
//     tft.fillScreen(TFT_BLACK);  // Clear the screen
//     tft.setTextSize(2);
//     tft.setCursor(10, 10);
//     tft.print("Temp: ");
//     tft.print(objectTemp);  // display the temperature
//     //--------------------------------------------------------------------

//     // Check if the readings are valid
//     if (isnan(ambientTemp) || isnan(objectTemp))
//     {
//       Serial.println("Failed to read from MLX90614 sensor!");
//       delay(2000);
//       return;
//     }

//     // Print sensor values for debugging
//     Serial.print("Ambient Temperature: ");
//     Serial.print(ambientTemp);
//     Serial.print(" °C, Object Temperature: ");
//     Serial.print(objectTemp);
//     Serial.println(" °C");

//     // Prepare the JSON payload
//     char jsonPayload[128];
//     snprintf(jsonPayload, sizeof(jsonPayload), "{\"ambientTemp\": %.2f, \"objectTemp\": %.2f}", ambientTemp, objectTemp);

//     // Open a connection to the server
//     WiFiClient client;
//     if (!client.connect(serverIP, serverPort))
//     {
//       Serial.println("Connection failed");
//       delay(2000);
//       return;
//     }

//     // Construct the HTTP POST request
//     String request = String("POST /data HTTP/1.1\r\n") +
//                      "Host: " + String(serverIP) + "\r\n" +
//                      "Content-Type: application/json\r\n" +
//                      "Content-Length: " + String(strlen(jsonPayload)) + "\r\n" +
//                      "Connection: close\r\n\r\n" +
//                      jsonPayload;

//     // Send the HTTP POST request
//     client.print(request);
//     Serial.println("Data sent to server: ");
//     Serial.println(request);

//     // Read the server's response (for debugging)
//     while (client.connected() || client.available())
//     {
//       if (client.available())
//       {
//         String response = client.readString();
//         Serial.println("Server response:");
//         Serial.println(response);
//       }
//     }

//     // Close the connection
//     client.stop();
//   }
//   if(currentTemperature >= TEMPERATURE_THRESHOLD){
//     digitalWrite(FAN_PIN, HIGH);
//   }else{
//     digitalWrite(FAN_PIN, LOW);
//   }
//   // And just stop, now that we've tried a download
//   // while (1)
//   //   ;
//   delay(100);
// }

// // void loop()
// // {

// //   // Read object and ambient temperatures
// //   float ambientTemp = mlx.readAmbientTempC();
// //   float objectTemp = mlx.readObjectTempC();

// //   // Check if the readings are valid
// //   if (isnan(ambientTemp) || isnan(objectTemp)) {
// //     Serial.println("Failed to read from MLX90614 sensor!");
// //     delay(2000);
// //     return;
// //   }

// //   // Print sensor values for debugging
// //   Serial.print("Ambient Temperature: ");
// //   Serial.print(ambientTemp);
// //   Serial.print(" °C, Object Temperature: ");
// //   Serial.print(objectTemp);
// //   Serial.println(" °C");

// //   // Prepare the JSON payload
// //   char jsonPayload[128];
// //   snprintf(jsonPayload, sizeof(jsonPayload), "{\"ambientTemp\": %.2f, \"objectTemp\": %.2f}", ambientTemp, objectTemp);

// //   // Open a connection to the server
// //   WiFiClient client;
// //   if (!client.connect(serverIP, serverPort)) {
// //     Serial.println("Connection failed");
// //     delay(2000);
// //     return;
// //   }

// //   // Construct the HTTP POST request
// //   String request = String("POST /data HTTP/1.1\r\n") +
// //                    "Host: " + String(serverIP) + "\r\n" +
// //                    "Content-Type: application/json\r\n" +
// //                    "Content-Length: " + String(strlen(jsonPayload)) + "\r\n" +
// //                    "Connection: close\r\n\r\n" +
// //                    jsonPayload;

// //   // Send the HTTP POST request
// //   client.print(request);
// //   Serial.println("Data sent to server: ");
// //   Serial.println(request);

// //   // Read the server's response (for debugging)
// //   while (client.connected() || client.available()) {
// //     if (client.available()) {
// //       String response = client.readString();
// //       Serial.println("Server response:");
// //       Serial.println(response);
// //     }
// //   }

// //   // Close the connection
// //   client.stop();

// //   // Wait before sending the next data
// //   delay(2000);
// // }
