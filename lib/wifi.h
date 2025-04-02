#ifndef WIFI_H
#define WIFI_H

#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"

// Declarações das funções
bool inicializar_wifi();
bool connect_to_wifi();
bool send_data_to_thingspeak(float db_level);
void timer_seconds(int seconds);


// Declaração da variável global
extern bool wifi_connected;

#endif // WIFI_H