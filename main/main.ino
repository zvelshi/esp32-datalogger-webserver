#include <SD_MMC.h>
#include <FS.h>

const uint8_t EN_PIN = 16;
const uint8_t ADC_PIN = 33;
const uint8_t ADC_READ_RESOLUTION = 12;
const uint8_t SAMPLING_INTERVAL = 10; 

uint16_t adcValue;

unsigned long timestamp;
uint32_t currentMillis;
uint32_t previousMillis = 0;

File dataFile;
bool ENState = false;
bool isRecording = false;

void startRecording(fs::FS &fs){
  int count = 0;
  String fileName;

  do {
    fileName = "/data-" + String(count) + ".csv";
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
 
void record(){
  adcValue = analogRead(ADC_PIN);
  timestamp = millis();
  
  dataFile.print(timestamp);
  dataFile.print(",");
  dataFile.println(adcValue);
  dataFile.flush();

  Serial.println(adcValue);
}

void setup() {
  Serial.begin(115200);

  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("MicroSD Card Mount Failed");
    return;
  }

  pinMode(ADC_PIN, INPUT);
  adcAttachPin(ADC_PIN);
  analogReadResolution(ADC_READ_RESOLUTION);

  pinMode(EN_PIN, INPUT_PULLUP);
  startRecording(SD_MMC);
}

void loop() {
  ENState = !digitalRead(EN_PIN);

  currentMillis = millis();

  if (ENState){
    if(((currentMillis - previousMillis) >= SAMPLING_INTERVAL) && isRecording){
      previousMillis = currentMillis;
      record();
    }
  } else if (ENState && isRecording) {
    stopRecording();
  }
}