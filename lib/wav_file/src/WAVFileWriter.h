#pragma once

#include <stdio.h>
#include <FS.h>
#include "WAVFile.h"

class WAVFileWriter
{
private:
  int m_file_size;

  FILE *m_fp;
  wav_header_t m_header;

public:
  WAVFileWriter(fs::File file, int sample_rate);
  void write(int16_t *samples, int count);
  void finish();
};
