#ifndef METEO_H_
#define METEO_H_

#include <Arduino.h>

String init_http(void);
float get_temp(void);
int get_WMO(void);
int get_day_night(void);

#endif