#ifndef WS2818B_H
#define WS2818B_H

#include "hardware/pio.h"

void ws2818b_program_init(PIO pio, uint sm, uint offset, uint pin, float freq);

#endif