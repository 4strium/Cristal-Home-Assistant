#include <SPI.h>
#include <SD.h>
#include <rec-sound.h>
#include <Arduino.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <I2SMEMSSampler.h>
#include <ADCSampler.h>
#include "SPIFFS.h"
#include <WAVFileWriter.h>
#include <config.h>

// Configuration de la carte SD
const int chipSelect = 5;

// Paramètres ADC
#define ADC_PIN 35
#define SAMPLE_RATE 16000 // Fréquence d'échantillonnage
#define RECORD_DURATION 3 // Durée en secondes
#define BUFFER_SIZE (SAMPLE_RATE*RECORD_DURATION)

// Taille de la fenêtre du filtre médian
#define MEDIAN_FILTER_WINDOW 5

static const char *TAG = "WAV";

// Structure de header pour fichier wav
struct wav_header {
  char riff[4];
  int32_t flength;
  char wave[4];
  char fmt[4];
  int32_t chunk_size;
  int16_t format_tag;
  int16_t num_chans;
  int32_t sample_rate;
  int32_t bytes_per_second;
  int16_t bytes_per_sample;
  int16_t bits_per_sample;
  char data[4];
  int32_t dlength;
};

const int header_length = sizeof(struct wav_header);

// Fichier sur la carte SD
File wavFile;
int16_t medianFilterBuffer[MEDIAN_FILTER_WINDOW];

void writeWavHeader(File file_inp){
  struct wav_header wavh;

  strncpy(wavh.riff, "RIFF", 4);
  strncpy(wavh.wave, "WAVE", 4);
  strncpy(wavh.fmt, "fmt ", 4); // Notez l'espace après 'fmt'
  strncpy(wavh.data, "data", 4);

  wavh.chunk_size = 16;
  wavh.format_tag = 1;
  wavh.num_chans = 1;
  wavh.sample_rate = SAMPLE_RATE;
  wavh.bits_per_sample = 16;
  wavh.bytes_per_second = wavh.sample_rate * wavh.bits_per_sample / 8 * wavh.num_chans;
  wavh.bytes_per_sample = wavh.bits_per_sample / 8 * wavh.num_chans;
  wavh.dlength = 0; // Sera mis à jour plus tard
  wavh.flength = 0; // Sera mis à jour plus tard

  file_inp.write((byte*)&wavh, header_length);
}

void updateWavHeader(File &file) {
  int fileSize = file.size();
  file.seek(4);
  file.write((byte)((fileSize - 8) & 0xFF));
  file.write((byte)(((fileSize - 8) >> 8) & 0xFF));
  file.write((byte)(((fileSize - 8) >> 16) & 0xFF));
  file.write((byte)(((fileSize - 8) >> 24) & 0xFF));
  file.seek(40);
  file.write((byte)((fileSize - 44) & 0xFF));
  file.write((byte)(((fileSize - 44) >> 8) & 0xFF));
  file.write((byte)(((fileSize - 44) >> 16) & 0xFF));
  file.write((byte)(((fileSize - 44) >> 24) & 0xFF));
}

int16_t medianFilter(int16_t newValue) {
  // Ajouter la nouvelle valeur dans le buffer
  for (int i = MEDIAN_FILTER_WINDOW - 1; i > 0; i--) {
    medianFilterBuffer[i] = medianFilterBuffer[i - 1];
  }
  medianFilterBuffer[0] = newValue;

  // Créer une copie du buffer pour le trier
  int16_t sortedBuffer[MEDIAN_FILTER_WINDOW];
  memcpy(sortedBuffer, medianFilterBuffer, sizeof(medianFilterBuffer));

  // Trier le buffer
  for (int i = 0; i < MEDIAN_FILTER_WINDOW - 1; i++) {
    for (int j = i + 1; j < MEDIAN_FILTER_WINDOW; j++) {
      if (sortedBuffer[i] > sortedBuffer[j]) {
        int16_t temp = sortedBuffer[i];
        sortedBuffer[i] = sortedBuffer[j];
        sortedBuffer[j] = temp;
      }
    }
  }

  // Retourner la valeur médiane
  return sortedBuffer[MEDIAN_FILTER_WINDOW / 2];
}

void record(I2SSampler *input, const char *fname)
{
  const int buffer_size = 1024; // Taille du buffer de lecture
  int16_t *samples = (int16_t *)malloc(sizeof(int16_t) * buffer_size);
  
  if (samples == NULL) {
    Serial.println("Erreur d'allocation mémoire pour les échantillons");
    return;
  }
  
  Serial.println("Démarrage de l'enregistrement");

  input->start(); // Démarrer l'acquisition audio

  SD.begin(chipSelect);
  // Ouvrir le fichier sur la carte SD en écriture binaire
  File file = SD.open(fname, FILE_WRITE);
  if (!file) {
    Serial.println("Erreur lors de l'ouverture du fichier %s");
    free(samples);
    return;
  }

  // Créer un écrivain de fichier WAV
  WAVFileWriter writer(file, input->sample_rate());

  // Enregistrer pendant la durée spécifiée
  int64_t start_time = esp_timer_get_time();
  while (esp_timer_get_time() - start_time < RECORD_DURATION * 1000000)
  {
    int samples_read = input->read(samples, buffer_size);
    writer.write(samples, samples_read);
  }

  // Arrêter l'acquisition audio
  input->stop();

  // Finaliser l'écriture du fichier WAV
  writer.finish();

  // Fermer le fichier
  file.close();

  // Libérer la mémoire et nettoyer
  free(samples);
  Serial.println("Enregistrement terminé");
}

void record_mic() {
  Serial.println("Démarrage de l'enregistrement");

  // Initialiser la carte SD
  if (!SD.begin(chipSelect)) {
    Serial.println("Erreur d'initialisation de la carte SD!");
    return;
  }

  // Créer un nouvel échantillonneur ADC
  I2SSampler *input = new ADCSampler(ADC_UNIT_1, ADC1_CHANNEL_7, i2s_adc_config);

  // Appeler la fonction record avec l'entrée et le nom du fichier
  record(input, "/audio.wav");

  // Nettoyer et libérer la mémoire
  delete input;

  Serial.println("Enregistrement terminé");
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