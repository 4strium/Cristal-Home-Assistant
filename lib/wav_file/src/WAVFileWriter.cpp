#include <FS.h>  // Inclure la bibliothèque pour la gestion des fichiers sur ESP32
#include "WAVFile.h"

class WAVFileWriter {
public:
  WAVFileWriter(fs::File file, int sample_rate);

  void write(int16_t *samples, int count);
  void finish();

private:
  fs::File m_file;
  wav_header_t m_header;
  int m_file_size;
};

WAVFileWriter::WAVFileWriter(fs::File file, int sample_rate) {
  m_file = file;
  m_header.sample_rate = sample_rate;
  // Écrire l'en-tête WAV initial dans le fichier
  m_file.write((uint8_t*)&m_header, sizeof(wav_header_t));
  m_file_size = sizeof(wav_header_t);
}

void WAVFileWriter::write(int16_t *samples, int count) {
  m_file.write((uint8_t*)samples, sizeof(int16_t) * count);
  m_file_size += sizeof(int16_t) * count;
}

void WAVFileWriter::finish() {
  // Mettre à jour l'en-tête avec les informations correctes sur la taille du fichier
  m_header.data_bytes = m_file_size - sizeof(wav_header_t);
  m_header.wav_size = m_file_size - 8;

  // Revenir au début du fichier pour écrire l'en-tête mis à jour
  m_file.seek(0);
  m_file.write((uint8_t*)&m_header, sizeof(wav_header_t));
}
