#ifndef __NEOPIXEL_INC
#define __NEOPIXEL_INC

// Inclusão de bibliotecas necessárias
#include <stdlib.h>          // Biblioteca padrão para alocação de memória
#include "lib/ws2818b.pio.h" // Biblioteca para controle dos LEDs WS2818B via PIO
#include "pico/stdlib.h"     // Biblioteca padrão para funções de delay e GPIO

// Definicoes de hardware
#define LED_PIN 25  // Pino do LED onboard do Raspberry Pi Pico
#define NEOPIXEL_PIN 7  // Pino conectado a matriz de LEDs NeoPixel
#define LED_COUNT 25  // Numero total de LEDs na matriz

// Definição de um pixel no formato GRB (Green, Red, Blue)
struct pixel_t {
  uint8_t G, R, B; // Cada pixel é composto por três valores de 8 bits (G, R, B)
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Renomeia "struct pixel_t" para "npLED_t" para maior clareza

// Declaração do buffer de pixels que formam a matriz de LEDs
static npLED_t *leds;      // Ponteiro para o buffer de LEDs
static uint led_count;     // Número total de LEDs na matriz

// Variáveis para controle da máquina de estado PIO
static PIO np_pio;         // PIO (Programmable I/O) usado para controlar os LEDs
static uint np_sm;         // Máquina de estado (SM) do PIO usada para enviar dados

/**
 * Inicializa a máquina PIO para controle da matriz de LEDs.
 * 
 * @param pin   Pino GPIO conectado ao fio de dados dos LEDs.
 * @param amount Número de LEDs na matriz.
 */
void npInit(uint pin, uint amount) {
  led_count = amount;  // Define o número de LEDs
  leds = (npLED_t *)calloc(led_count, sizeof(npLED_t)); // Aloca memória para o buffer de LEDs

  // Adiciona o programa PIO para controle dos LEDs WS2818B
  uint offset = pio_add_program(pio0, &ws2818b_program);
  np_pio = pio0;  // Usa o PIO0 como padrão

  // Toma posse de uma máquina de estado (SM) no PIO
  np_sm = pio_claim_unused_sm(np_pio, false);
  if (np_sm < 0) {
    // Se nenhuma máquina estiver livre no PIO0, tenta no PIO1
    np_pio = pio1;
    np_sm = pio_claim_unused_sm(np_pio, true); // Se ainda não houver máquinas, entra em pânico
  }

  // Inicializa o programa PIO na máquina de estado obtida
  ws2818b_program_init(np_pio, np_sm, offset, pin, 800000.f);

  // Limpa o buffer de pixels, definindo todos os LEDs como desligados (0, 0, 0)
  for (uint i = 0; i < led_count; ++i) {
    leds[i].R = 0;
    leds[i].G = 0;
    leds[i].B = 0;
  }
}

/**
 * Define a cor de um LED específico no buffer.
 * 
 * @param index Índice do LED na matriz (começando em 0).
 * @param r     Valor de vermelho (0 a 255).
 * @param g     Valor de verde (0 a 255).
 * @param b     Valor de azul (0 a 255).
 */
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
  leds[index].R = r;  // Define o valor de vermelho
  leds[index].G = g;  // Define o valor de verde
  leds[index].B = b;  // Define o valor de azul
}

/**
 * Limpa o buffer de pixels, desligando todos os LEDs.
 */
void npClear() {
  for (uint i = 0; i < led_count; ++i)
    npSetLED(i, 0, 0, 0);  // Define todos os LEDs como desligados (0, 0, 0)
}

/**
 * Envia os dados do buffer para os LEDs, atualizando a matriz.
 */
void npWrite() {
  // Envia cada valor de 8 bits (G, R, B) para a máquina de estado PIO
  for (uint i = 0; i < led_count; ++i) {
    pio_sm_put_blocking(np_pio, np_sm, leds[i].G);  // Envia o valor de verde
    pio_sm_put_blocking(np_pio, np_sm, leds[i].R);  // Envia o valor de vermelho
    pio_sm_put_blocking(np_pio, np_sm, leds[i].B);  // Envia o valor de azul
  }
  sleep_us(100);  // Aguarda 100us, conforme especificado no datasheet para o sinal de RESET
}

/**
 * Define a cor dos LEDs com base no nivel de volume em dB.
 */
void set_led_color_based_on_volume(float db) {
  npClear(); // Limpa a matriz de LEDs antes de definir novas cores

  // Define a cor dos LEDs de acordo com o nivel de dB
  if (db < 30) {
      for (uint i = 0; i < LED_COUNT; ++i) {
          npSetLED(i, 0, 0, 80); // Azul claro
      }
  } else if (db < 43) {
      for (uint i = 0; i < LED_COUNT; ++i) {
          npSetLED(i, 0, 0, 80); // Azul mais claro
      }
  } else if (db < 55) {
      for (uint i = 0; i < LED_COUNT; ++i) {
          npSetLED(i, 0, 80, 0); // Verde escuro
      }
  } else if (db < 60) {
      for (uint i = 0; i < LED_COUNT; ++i) {
          npSetLED(i, 80, 0, 0); // Vermelho
      }
  } else {
      for (uint i = 0; i < LED_COUNT; ++i) {
          npSetLED(i, 80, 0, 0); // Vermelho para sons muito altos
      }
  }

  npWrite(); // Atualiza a matriz de LEDs com as cores definidas
}

// Funcao para limpar a matriz de LEDs NeoPixel
void limpar_matriz_led() {
  for (int i = 0; i < LED_COUNT; i++) {
      npSetLED(i, 0, 0, 0);  // Define todas as cores dos LEDs como 0 (apagado)
  }
  npWrite();  // Atualiza a matriz de LEDs
}

#endif