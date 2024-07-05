#include <Arduino.h>
#include <stdio.h>
#include <pwm.h>

#define RED_LED 14
#define BLUE_LED 12


void vari_red(void *pvParameters){
    pinMode(RED_LED, OUTPUT);
    for(int pwm_loop = 0; pwm_loop < 2; pwm_loop++){
        for (int fadeValue =15; fadeValue <= 255; fadeValue ++){
            analogWrite(RED_LED, fadeValue);
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

        // Variation du max au min par décrément
        for (int fadeValue =255; fadeValue >= 15; fadeValue --){
            analogWrite(RED_LED, fadeValue);
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }
    analogWrite(RED_LED, 255);
}

void vari_blue(void *pvParameters){
    pinMode(BLUE_LED, OUTPUT);
    for(int pwm_loop = 0; pwm_loop < 2; pwm_loop++){
        for (int fadeValue =15; fadeValue <= 255; fadeValue ++){
            analogWrite(BLUE_LED, fadeValue);
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

        // Variation du max au min par décrément
        for (int fadeValue =255; fadeValue >= 15; fadeValue --){
            analogWrite(BLUE_LED, fadeValue);
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }
    analogWrite(BLUE_LED, 255);
}

void fade_out_red(void *pvParameters) {
    pinMode(RED_LED, OUTPUT);
    // Variation du max au min par décrément
    for (int fadeValue =255; fadeValue >= 0; fadeValue --){
       analogWrite(RED_LED, fadeValue);
       vTaskDelay(2 / portTICK_PERIOD_MS);
    }
}

void fade_in_blue(void *pvParameters){
    pinMode(BLUE_LED, OUTPUT);
    for (int fadeValue =0; fadeValue <= 255; fadeValue ++){
        analogWrite(BLUE_LED, fadeValue);
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void fade_out_blue(void *pvParameters){
    pinMode(BLUE_LED, OUTPUT);
    for (int fadeValue =255; fadeValue >= 0; fadeValue --){
        analogWrite(BLUE_LED, fadeValue);
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
}