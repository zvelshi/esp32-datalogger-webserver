#include <WiFi.h>
#include <SD_MMC.h>
#include <FS.h>

const uint8_t ADC_PIN = 33;
const uint8_t ADC_READ_RESOLUTION = 12;
const uint16_t SAMPLING_INTERVAL = 500; //cannot exceed 65536

const char* SSID = "ESP32-AP";
const char* PASSWORD = "123test123";

uint16_t adcValue;
uint32_t timestamp;
uint32_t currentMillis;
uint32_t previousMillis;

WiFiServer server(80);
String header;

bool isRecording = false;
File dataFile;

void listFiles(WiFiClient client, fs::FS &fs, const String& path){
  File root = fs.open(path, "r");
  if(!root || !root.isDirectory()){
    Serial.println("Failed to open directory");
    return;
  }

  client.println("<h2>Files on SD Card</h2><ul>");

  File entry = root.openNextFile();
  while (entry) {
    client.print("<li>" + String(path) + String(entry.name()) + "</li>");// - <a href=\"/download?file=" + String(path) + "/" + String(entry.name()) + "\">Download</a></li>");
    entry = root.openNextFile();
  }

  client.println("</ul>");
  entry.close();
  root.close();
}

/*
void downloadFile(WiFiClient client, fs::FS &fs, const String& path) {
  File file = fs.open(path, FILE_READ);
  if (!file) {
    Serial.println("File not found: " + path);
    client.println("HTTP/1.1 404 Not Found");
    return;
  }

  String filename = path.substring(path.lastIndexOf('/') + 1);
  String contentType = "text/csv";

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Disposition: attachment; filename=\"" + filename + "\"");
  client.println("Content-type: " + contentType);
  client.println("Connection: close");
  client.println();

  while (file.available()) {
    client.write(file.read());
  }

  file.close();
}
*/


void startRecording(fs::FS &fs){
  int count = 0;
  String fileName;

  do {
    fileName = "/data(" + String(count) + ").csv";
    count++;
  } while (fs.exists(fileName));

  dataFile = fs.open(fileName, FILE_WRITE);
  if(!dataFile){
    Serial.println("Failed to create file.");
    return;
  }

  dataFile.println("Timestamp,Value");
  isRecording = true;
}

void stopRecording(){
  isRecording = false;
  dataFile.close();
}

void recordData(){
  if(isRecording){
    adcValue = analogRead(ADC_PIN);
    timestamp = millis();

    dataFile.print(timestamp);
    dataFile.print(",");
    dataFile.println(adcValue);

    dataFile.flush();
    Serial.println(adcValue);
  }
}

void setup() {
  Serial.begin(115200);

  //Mount MicroSD Card
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("MicroSD Card Mount Failed");
    return;
  }

  //Setup AP
  Serial.print("Setting up AP... ");
  WiFi.softAP(SSID, PASSWORD);
  IPAddress IP = WiFi.softAPIP();

  //Print IP to console
  Serial.print("Address: ");
  Serial.println(IP);
  server.begin();

  //Configure ADCI
  pinMode(ADC_PIN, INPUT);
  adcAttachPin(ADC_PIN);
  analogReadResolution(ADC_READ_RESOLUTION);
  
  randomSeed(analogRead(ADC_PIN));
  previousMillis = 0;
}

void loop() {
  WiFiClient client = server.available();
  if(client){
    Serial.println("New client.");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // start/stop recording
            if (header.indexOf("GET /start") >= 0){
              isRecording = true;
              startRecording(SD_MMC);
            } else if (header.indexOf("GET /stop") >= 0) {
              isRecording = false;
              stopRecording();
            } /* else if (header.indexOf("GET /download?file=") >= 0){
              String path = header.substring(header.indexOf("file=") + 5, header.indexOf(" HTTP/1.1"));
              downloadFile(client, SD_MMC, path);
            } */

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println("ul {list-style-type: none;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            client.println("<body><h1>ESP32 Web Server</h1>");

            // Add buttons for start/stop recording
            client.println("<p>Recording Status: " + String(isRecording ? "Recording" : "Not Recording") + "</p>");
            if (isRecording) {
              client.println("<p><a href=\"/stop\"><button class=\"button button2\">Stop Recording</button></a></p>");
            } else {
              client.println("<p><a href=\"/start\"><button class=\"button\">Start Recording</button></a></p>");
            }

            // Display the list of files on the SD card
            listFiles(client, SD_MMC, "/");

            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    header = "";
    client.stop();
    Serial.println("Client disconnected.");
  }

  currentMillis = millis();
  if((currentMillis - previousMillis) >= SAMPLING_INTERVAL){
    previousMillis = currentMillis;
    recordData();
  }
}