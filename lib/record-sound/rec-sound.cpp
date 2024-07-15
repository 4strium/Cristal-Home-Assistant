#include <SPI.h>
#include <SD.h>
#include <rec-sound.h>

// Définir la broche d'entrée pour le MAX9814
const int micPin = 35;

// Configuration de la carte SD
const int chipSelect = 5;

// Durée de l'enregistrement en secondes
const int recordDuration = 4;
const int sampleRate = 16000;  // Taux d'échantillonnage en Hz (ex: 16000 échantillons par seconde)

// Fichier sur la carte SD
File audioFile;

void record_mic() {
  // Initialisation de la carte SD
  if (!SD.begin(chipSelect)) {
    Serial.println("Erreur d'initialisation de la carte SD!");
    while (1);
  }

  Serial.println("Carte SD initialisée.");

  // Créer un nouveau fichier pour enregistrer les données audio
  audioFile = SD.open("/audio.wav", FILE_WRITE);
  if (!audioFile) {
    Serial.println("Erreur de création du fichier audio!");
    while (1);
  }

  Serial.println("Fichier audio créé.");

  // Démarrer l'enregistrement
  Serial.println("Début de l'enregistrement...");
  unsigned long startTime = millis();

  while (millis() - startTime < recordDuration * 1000) {
    // Lire la valeur analogique du microphone
    int sample = analogRead(micPin);
    
    // Convertir la valeur en format 8 bits (optionnel, selon le format désiré)
    byte audioSample = map(sample, 0, 4095, 0, 255);

    // Écrire l'échantillon dans le fichier
    audioFile.write(audioSample);

    // Attendre pour respecter le taux d'échantillonnage
    delayMicroseconds(1000000 / sampleRate);
  }

  // Terminer l'enregistrement
  audioFile.close();
  Serial.println("Enregistrement terminé.");
}

void delete_audio_file() {
  // Initialisation de la carte SD
  if (!SD.begin(chipSelect)) {
    Serial.println("Erreur d'initialisation de la carte SD!");
    while (1);
  }

  Serial.println("Carte SD initialisée.");

  // Supprimer le fichier audio
  if (SD.exists("/audio.wav")) {
    if (SD.remove("/audio.wav")) {
      Serial.println("Fichier audio.wav supprimé.");
    } else {
      Serial.println("Erreur lors de la suppression du fichier audio.wav.");
    }
  } else {
    Serial.println("Fichier audio.wav n'existe pas.");
  }
}