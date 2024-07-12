#include <WiFi.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <wavserv.h>

// Configuration du serveur
const char* serverIP = "178.128.174.115";
const int serverPort = 8080;

// Pins SPI
#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5

#define BUFFER_SIZE 4096

String recowav(){
    // Initialisation du SPI et de la carte SD
    SPI.begin(SCK, MISO, MOSI, CS);
    if (!SD.begin(CS)) {
        Serial.println("Card Mount Failed");
        return "error";
    }
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return "error";
    }

    // Création du socket
    WiFiClient client;
    if (!client.connect(serverIP, serverPort)) {
        Serial.println("Connection to server failed");
        return "error";
    }

    // Ouverture du fichier .wav à envoyer
    File file = SD.open("/audio.wav");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return "error";
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
    file.close();

    // Fermer l'écriture du socket pour indiquer la fin de l'envoi du fichier
    client.flush();
    client.stop();

    // Reconnecter pour recevoir le rapport
    if (!client.connect(serverIP, serverPort)) {
        Serial.println("Connection to server failed");
        return "error";
    }

    // Réception du contenu de rapport.txt
    Serial.println("Waiting for the server's report...");
    while (client.connected() || client.available()) {
        if (client.available()) {
            String report = client.readStringUntil('\n');  // Read until new line character
            Serial.print(report);  // Print the report content to the Serial monitor
        }
    }
  
    // Fermeture du socket
    client.stop();
    Serial.println("\nFile sent and report received successfully");
}