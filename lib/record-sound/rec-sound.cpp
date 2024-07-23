#include <SPI.h>
#include <SD.h>
#include <rec-sound.h>
#include <driver/i2s.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

// Configuration de la carte SD
const int chipSelect = 5;

// Paramètres I2S
#define I2S_SAMPLE_RATE 16000
#define I2S_NUM_CHANNELS 1
#define I2S_BITS_PER_SAMPLE 16

#define RECORD_DURATION 3 // Durée en secondes

// Analog Microphone Settings - ADC1_CHANNEL_7 is GPIO35
#define ADC_MIC_CHANNEL ADC1_CHANNEL_7

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

void writeWavHeader(File &file_inp){
  struct wav_header wavh;

  strncpy(wavh.riff, "RIFF", 4);
  strncpy(wavh.wave, "WAVE", 4);
  strncpy(wavh.fmt, "fmt ", 4); // Notez l'espace après 'fmt'
  strncpy(wavh.data, "data", 4);

  wavh.chunk_size = 16;
  wavh.format_tag = 1;
  wavh.num_chans = 1;
  wavh.sample_rate = I2S_SAMPLE_RATE;
  wavh.bits_per_sample = I2S_BITS_PER_SAMPLE;
  wavh.bytes_per_second = (wavh.sample_rate * wavh.bits_per_sample * wavh.num_chans) / 8 ;
  wavh.bytes_per_sample = (wavh.bits_per_sample * wavh.num_chans) / 8;
  wavh.dlength = 0; // Sera mis à jour plus tard
  wavh.flength = 36 + wavh.dlength; // Sera mis à jour plus tard

  file_inp.write((byte*)&wavh, header_length);
}

void updateWavHeader(File &file) {
  uint32_t fileSize = file.size();
  uint32_t dataSize = fileSize - header_length;

  file.seek(4);
  file.write((uint8_t*)&fileSize, 4); // Mettre à jour flength

  file.seek(40);
  file.write((uint8_t*)&dataSize, 4); // Mettre à jour dlength
}

void setupI2S() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0};

  //install and start i2s driver
  i2s_driver_install(I2S_NUM_0, &i2s_config, 4, NULL);

  //init ADC pad
  i2s_set_adc_mode(ADC_UNIT_1, ADC_MIC_CHANNEL);

  // enable the ADC
  i2s_adc_enable(I2S_NUM_0);
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

  setupI2S();

  Serial.println("Début de l'enregistrement...");
  
  size_t bytes_read = 0;
  int16_t i2s_samples[1024]; // Buffer for I2S samples

  unsigned long startMicros = micros();

  while (micros() - startMicros < (RECORD_DURATION * 1000000)) {
    i2s_read(I2S_NUM_0, (int16_t*)i2s_samples, 1024*sizeof(int16_t), &bytes_read, portMAX_DELAY);
    int samples_read = bytes_read / sizeof(int16_t);
    for (int i = 0; i < samples_read; i++)
    {
        i2s_samples[i] = (2048 - (uint16_t(i2s_samples[i]) & 0xfff)) * 15;
    }
    
    wavFile.write((u_int8_t*)i2s_samples, bytes_read);

  }

  // Mettre à jour la taille des données dans l'entête WAV
  updateWavHeader(wavFile);

  wavFile.close();

  i2s_driver_uninstall(I2S_NUM_0);

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