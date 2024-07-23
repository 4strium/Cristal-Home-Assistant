#ifndef REC_H_
#define REC_H_

#include <SPI.h>
#include <SD.h>

void record_mic();
void delete_audio_file();
void setupI2S();
void updateWavHeader(File &file);
void writeWavHeader(File &file_inp);

#endif