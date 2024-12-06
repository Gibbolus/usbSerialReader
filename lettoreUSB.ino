#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 rfid(SS_PIN, RST_PIN);
bool isScanning = false;
unsigned long startScanTime = 0;
const unsigned long SCAN_TIMEOUT = 15000;

void setup() {
  Serial.begin(9600);
  SPI.begin();

  pinMode(SS_PIN, OUTPUT);
  digitalWrite(SS_PIN, HIGH);

  rfid.PCD_Init();
  delay(500);
  
  // Invia un identificatore univoco per questo specifico scanner
  Serial.println("CUSTOM_RFID_SCANNER_ID:SCANNER_2024_V1");
  delay(500);
  Serial.println("START");
  delay(500);
  Serial.println("READY");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "CHECK_DEVICE") {
      Serial.println("CUSTOM_RFID_SCANNER_ID:SCANNER_2024_V1");
    }
    else if (command == "SCAN") {
      isScanning = true;
      startScanTime = millis();
    }
  }

  if (isScanning) {
    if (millis() - startScanTime >= SCAN_TIMEOUT) {
      isScanning = false;
      Serial.println("TIMEOUT");
      return;
    }

    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
      unsigned long uidDecimal = 0;
      for (byte i = 0; i < rfid.uid.size; i++) {
        uidDecimal = uidDecimal * 256 + rfid.uid.uidByte[i];
      }

      String formattedUID = String(uidDecimal);
      while (formattedUID.length() < 10) {
        formattedUID = "0" + formattedUID;
      }
      Serial.print("CARD:");
      Serial.println(formattedUID);
      isScanning = false;
      rfid.PICC_HaltA();
    }
  }

  delay(100);
}