#include <WiFi.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

// Configuration du serveur
const char* serverIP = "178.128.174.115";
const int serverPort = 8080;

// Pins SPI
#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5

#define BUFFER_SIZE 1024

void setup() {
  Serial.begin(115200);

  Serial.println("Entrez le SSID de votre réseau Wifi :");
  while (!Serial.available()) {
    // Wait for user input
  }
  String ssid = Serial.readString();

  Serial.println("Entrez le mot-de-passe de votre réseau Wifi :");
  while (!Serial.available()) {
    // Wait for user input
  }
  String password = Serial.readString();

  const char* ssid_cstr = ssid.c_str();
  const char* password_cstr = password.c_str();

  // Connexion WiFi
  WiFi.begin(ssid_cstr, password_cstr);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  // Initialisation du SPI et de la carte SD
  SPI.begin(SCK, MISO, MOSI, CS);
  if (!SD.begin(CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  // Création du socket
  WiFiClient client;
  if (!client.connect(serverIP, serverPort)) {
    Serial.println("Connection to server failed");
    return;
  }

  // Ouverture du fichier .wav à envoyer
  File file = SD.open("/audio.wav");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  // Lecture et envoi du fichier .wav
  char buffer[BUFFER_SIZE];
  while (file.available()) {
    int bytesRead = file.read((uint8_t*)buffer, BUFFER_SIZE);
    if (client.write((const uint8_t*)buffer, bytesRead) != bytesRead) {
      Serial.println("Error sending file");
      break;
    }
  }

  // Fermeture du socket et du fichier
  file.close();
  client.stop();

  Serial.println("File sent successfully");
}

void loop() {}