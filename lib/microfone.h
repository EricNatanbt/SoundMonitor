#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include <math.h>

// Definições de pinos e constantes
#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)
#define ADC_CLOCK_DIV 48.f
#define SAMPLES 400
#define ADC_ADJUST(x) ((x) * 3.3f / (1 << 12u) - 1.65f)
#define ADC_MAX 3.3f
#define ADC_STEP (3.3f/5.f)
#define FILTER_SIZE 5

// Declarações de funções
void microphone_init();
void sample_mic();
float mic_power();
float apply_moving_average_filter(float new_value);
float calculate_db(float voltage);
const char* classify_volume(float db);

#endif // MICROPHONE_H