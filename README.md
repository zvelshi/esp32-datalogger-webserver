# ESP32-DataLogger-WebServer
This repository contains the source code for an ESP32-based project that combines a web server with data logging capabilities. The project utilizes the ESP32 microcontroller, an SD card module, and Wi-Fi connectivity to create a web interface for controlling data recording and downloading recorded data files.
## Purpose
The main purpose of this project is to showcase how to create a web server on the ESP32 platform that allows users to remotely control the process of recording analog data and downloading the recorded data files. The project demonstrates the following key features:
- Starting and stopping the data recording process remotely via the web interface.
- Displaying the recording status (whether the data is currently being recorded or not).
- Providing a list of recorded data files available for download.
- Allowing users to download the recorded data files in CSV format.

## Technologies Used
- **ESP32 Microcontroller:** The project is built around the ESP32, a powerful and versatile microcontroller that provides Wi-Fi connectivity and sufficient processing capabilities for the web server and data logging tasks.
- **Arduino Framework:** The code is written using the Arduino programming environment, making it accessible and easy to understand for users familiar with Arduino development.
- **WiFi Library:** The `WiFi.h` library is used to establish a soft access point (AP) for users to connect to the ESP32 and interact with the web interface.
- **SD Card Module:** The project utilizes the `SD_MMC.h` library to interface with an SD card module. This allows recorded data to be saved to the SD card in CSV format.

## Getting Started
To set up and run the project:
1. **Hardware Setup:** Connect the ESP32 and the SD card module according to the hardware configuration defined in the code.
2. **Software Setup:** Upload the provided code to the ESP32 using the Arduino IDE. Ensure that the necessary libraries (`WiFi.h`, `SD_MMC.h`, and `FS.h`) are installed.
3. **Access the Web Interface:** Connect to the Wi-Fi network hosted by the ESP32 (SSID: ESP32-AP) and open a web browser. Enter the IP address displayed on the serial monitor to access the web interface.
4. **Web Interface:**
   - The web interface displays the recording status and provides buttons to start or stop data recording.
   - It also lists the recorded data files available for download.
5. **Data Recording and Download:**
   - Start recording by clicking the "Start Recording" button.
   - Stop recording by clicking the "Stop Recording" button.
   - Recorded data files can be downloaded by clicking the "Download" links.
