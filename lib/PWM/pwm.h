#ifndef PWM_H_
#define PWM_H_

#include <Arduino.h>

void vari_red(void *pvParameters);
void fade_out_red(void *pvParameters);

void vari_blue(void *pvParameters);
void fade_in_blue(void *pvParameters);
void fade_out_blue(void *pvParameters);

#endif