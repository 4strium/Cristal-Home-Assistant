#include <SPI.h>
#include <SD.h>
#include <rec-sound.h>

// Configuration de la carte SD
const int chipSelect = 5;

// Paramètres ADC
#define ADC_PIN 35
#define SAMPLE_RATE 16000 // Fréquence d'échantillonnage
#define RECORD_DURATION 4 // Durée en secondes

// Fichier sur la carte SD
File wavFile;

void writeWavHeader(File file, int sampleRate, int bitsPerSample, int channels, int dataSize) {
  byte header[44];
  long totalDataLen = dataSize + 36;
  long audioDataLen = dataSize;
  long longSampleRate = sampleRate;
  long byteRate = sampleRate * channels * bitsPerSample / 8;

  // ChunkID "RIFF"
  header[0] = 'R'; header[1] = 'I'; header[2] = 'F'; header[3] = 'F';
  // ChunkSize
  header[4] = (byte) (totalDataLen & 0xff); header[5] = (byte) ((totalDataLen >> 8) & 0xff); header[6] = (byte) ((totalDataLen >> 16) & 0xff); header[7] = (byte) ((totalDataLen >> 24) & 0xff);
  // Format "WAVE"
  header[8] = 'W'; header[9] = 'A'; header[10] = 'V'; header[11] = 'E';
  // Subchunk1ID "fmt "
  header[12] = 'f'; header[13] = 'm'; header[14] = 't'; header[15] = ' ';
  // Subchunk1Size (16 for PCM)
  header[16] = 16; header[17] = 0; header[18] = 0; header[19] = 0;
  // AudioFormat (1 for PCM)
  header[20] = 1; header[21] = 0;
  // NumChannels
  header[22] = channels; header[23] = 0;
  // SampleRate
  header[24] = (byte) (longSampleRate & 0xff); header[25] = (byte) ((longSampleRate >> 8) & 0xff); header[26] = (byte) ((longSampleRate >> 16) & 0xff); header[27] = (byte) ((longSampleRate >> 24) & 0xff);
  // ByteRate
  header[28] = (byte) (byteRate & 0xff); header[29] = (byte) ((byteRate >> 8) & 0xff); header[30] = (byte) ((byteRate >> 16) & 0xff); header[31] = (byte) ((byteRate >> 24) & 0xff);
  // BlockAlign
  header[32] = (byte) (channels * bitsPerSample / 8); header[33] = 0;
  // BitsPerSample
  header[34] = bitsPerSample; header[35] = 0;
  // Subchunk2ID "data"
  header[36] = 'd'; header[37] = 'a'; header[38] = 't'; header[39] = 'a';
  // Subchunk2Size
  header[40] = (byte) (audioDataLen & 0xff); header[41] = (byte) ((audioDataLen >> 8) & 0xff); header[42] = (byte) ((audioDataLen >> 16) & 0xff); header[43] = (byte) ((audioDataLen >> 24) & 0xff);

  file.write(header, sizeof(header));
}

void updateWavHeader(File &file) {
  int fileSize = file.size();
  file.seek(4);
  file.write((byte)((fileSize - 8) & 0xFF)); file.write((byte)(((fileSize - 8) >> 8) & 0xFF)); file.write((byte)(((fileSize - 8) >> 16) & 0xFF)); file.write((byte)(((fileSize - 8) >> 24) & 0xFF));
  file.seek(40);
  file.write((byte)((fileSize - 44) & 0xFF)); file.write((byte)(((fileSize - 44) >> 8) & 0xFF)); file.write((byte)(((fileSize - 44) >> 16) & 0xFF)); file.write((byte)(((fileSize - 44) >> 24) & 0xFF));
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
  writeWavHeader(wavFile, SAMPLE_RATE, 16, 1, 0);

  Serial.println("Début de l'enregistrement...");
  
  int samples = RECORD_DURATION * SAMPLE_RATE;
  int16_t sample;

  for (int i = 0; i < samples; i++) {
    sample = analogRead(ADC_PIN);
    sample = (sample - 2048) * 16; // Ajuster l'échelle de 12 bits à 16 bits
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