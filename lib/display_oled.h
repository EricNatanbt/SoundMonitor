#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H

#include "pico/stdlib.h"
#include "inc/ssd1306.h" // Inclua a biblioteca SSD1306
#include <string.h>
#include <stdio.h>

// Declarações das funções
void inicializa();
void print_texto(char *msg, uint32_t x, uint32_t y, uint32_t scale);
void print_linha(int x1, int y1, int x2, int y2);
void print_retangulo(int x1, int y1, int x2, int y2);
void limpar_tela();
void exibir_barra_carregamento(int porcentagem);
void exibir_tela_desligar(void);
void exibir_tela_pronto(void);
void timer_milliseconds(int milliseconds);


#endif // DISPLAY_OLED_H