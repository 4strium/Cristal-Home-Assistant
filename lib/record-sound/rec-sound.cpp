#include <SPI.h>
#include <SD.h>
#include <rec-sound.h>

// Configuration de la carte SD
const int chipSelect = 5;

// Paramètres ADC
#define ADC_PIN 35
#define SAMPLE_RATE 16000 // Fréquence d'échantillonnage
#define RECORD_DURATION 3 // Durée en secondes
#define BUFFER_SIZE SAMPLE_RATE*RECORD_DURATION

// Taille de la fenêtre du filtre médian
#define MEDIAN_FILTER_WINDOW 5

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
  file.write((byte)((fileSize - 8) & 0xFF)); file.write((byte)(((fileSize - 8) >> 8) & 0xFF)); file.write((byte)(((fileSize - 8) >> 16) & 0xFF)); file.write((byte)(((fileSize - 8) >> 24) & 0xFF));
  file.seek(40);
  file.write((byte)((fileSize - 44) & 0xFF)); file.write((byte)(((fileSize - 44) >> 8) & 0xFF)); file.write((byte)(((fileSize - 44) >> 16) & 0xFF)); file.write((byte)(((fileSize - 44) >> 24) & 0xFF));
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

void record_mic() {
  if (!SD.begin(chipSelect)) {
    Serial.println("Erreur d'initialisation de la carte SD!");
    while (1);
  }

  // Créer un nouveau fichier WAV
  wavFile = SD.open("/audio.wav", FILE_WRITE);
  if (!wavFile) {
    Serial.println("Erreur lors de la création du fichier WAV");
    return;
  }

  // Préparer l'en-tête WAV sans la taille des données, qui sera mise à jour après l'enregistrement
  writeWavHeader(wavFile);

  Serial.println("Début de l'enregistrement...");
  
  int samples = RECORD_DURATION * SAMPLE_RATE;
  int16_t sample;

  for (int i = 0; i < samples; i++) {
    sample = analogRead(ADC_PIN);
    sample = (sample - 2048) * 16; // Ajuster l'échelle de 12 bits à 16 bits
    sample = medianFilter(sample);
    wavFile.write((byte*)&sample, sizeof(sample));
    delayMicroseconds(1000000 / SAMPLE_RATE);
  }

  // Mettre à jour la taille des données dans l'entête WAV
  updateWavHeader(wavFile);

  wavFile.close();

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