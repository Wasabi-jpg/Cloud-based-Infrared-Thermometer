# Cloud-based-Infrared-Thermometer

Hi, this is an IoT project where a friend and I wanted to improve on the commercial infrared thermometer. There's two main parts, the first being connectivity to the cloud with an ESP32 microcontroller to send temperature data up via WiFi. The second is a proof of concept of a responsive hospital room, where upon a high temperature reading received via the ESP-NOW communication protocol, a second ESP32 will activate an A/C system to cool down a patient.

## Parts Used
- 2 ESP32 boards (Lilygo TTGO)
- 1 GY-906 MLX90614 Non-Contact IR Infrared Temperature Sensor Module
- 1 MF50100V1-1000U-A99 (Fan)

## Setup Image
![github_pic](https://github.com/user-attachments/assets/84910e85-ef39-4a21-8774-44d712213e06)
