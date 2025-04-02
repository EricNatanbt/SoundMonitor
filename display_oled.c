#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"

// Definições do barramento I2C e pinos de conexão do display OLED
#define I2C_PORT i2c1
#define PINO_SCL 14
#define PINO_SDA 15

ssd1306_t disp; // Estrutura do display OLED

// Função de temporizador que substitui sleep_ms
void timer_milliseconds(int milliseconds) {
    uint64_t start_time = time_us_64(); // Obtém o tempo atual em microssegundos
    uint64_t delay = milliseconds * 1000; // Converte milissegundos para microssegundos

    // Aguarda até que o tempo especificado tenha passado
    while (time_us_64() - start_time < delay) {
        // Loop vazio (busy-wait)
    }
}

// Função para inicializar o sistema e o display OLED
void inicializa(){
    stdio_init_all(); // Inicializa a comunicação padrão
    i2c_init(I2C_PORT, 400*1000); // Inicializa o I2C com frequência de 400kHz
    gpio_set_function(PINO_SCL, GPIO_FUNC_I2C);
    gpio_set_function(PINO_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_SCL);
    gpio_pull_up(PINO_SDA);
    disp.external_vcc = false; // Usa a alimentação interna do OLED
    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT); // Inicializa o display OLED
}

// Função para exibir um texto na tela OLED
void print_texto(char *msg, uint32_t x, uint32_t y, uint32_t scale) {
    ssd1306_draw_string(&disp, x, y, scale, msg); // Desenha o texto na posição e escala escolhidas
    ssd1306_show(&disp); // Atualiza o display para exibir o texto
}

// Função para limpar a tela do display OLED
void limpar_tela(){
    ssd1306_clear(&disp); // Limpa a tela
}

// Função para desenhar uma linha no display OLED
void print_linha(int x1, int y1, int x2, int y2){
    ssd1306_draw_line(&disp, x1, y1, x2, y2); // Desenha uma linha entre dois pontos
    ssd1306_show(&disp); // Atualiza o display
}

// Função para desenhar um retângulo no display OLED
void print_retangulo(int x1, int y1, int x2, int y2){
    ssd1306_draw_empty_square(&disp, x1, y1, x2, y2); // Desenha um retângulo vazio
    ssd1306_show(&disp); // Atualiza o display
}

// Função para exibir a barra de carregamento
void exibir_barra_carregamento(int porcentagem) {
    char barra[12] = "[          ]"; // Representação gráfica da barra de carregamento
    int posicoes_preenchidas = porcentagem / 10;

    // Preenche a barra conforme a porcentagem
    for (int i = 1; i <= posicoes_preenchidas; i++) {
        barra[i] = '#';
    }

    // Atualiza o display OLED com a barra de carregamento
    limpar_tela();
    print_texto("SOUND", 1, 1, 2); // Título 1
    print_texto("MONITOR", 20, 17, 2); // Título 2
    print_texto("Inicializando...", 20, 33, 1); // Mensagem de status
    print_texto(barra, 5, 52, 1); // Barra de carregamento gráfica
    char porcentagem_str[16];
    sprintf(porcentagem_str, "%d%%", porcentagem); // Converte a porcentagem para string
}

// Função para exibir a tela de desligamento
void exibir_tela_desligar(void) {
    for (int i = 0; i < 2; i++) {
        limpar_tela();
        print_texto("SOUND", 1, 1, 2);
        print_texto("MONITOR", 20, 17, 2);
        print_texto("Desligando.", 30, 40, 1);
        timer_milliseconds(520);
        limpar_tela();
        print_texto("SOUND", 1, 1, 2);
        print_texto("MONITOR", 20, 17, 2);
        print_texto("Desligando..", 30, 40, 1);
        timer_milliseconds(520);
        limpar_tela();
        print_texto("SOUND", 1, 1, 2);
        print_texto("MONITOR", 20, 17, 2);
        print_texto("Desligando...", 30, 40, 1);
        timer_milliseconds(520);
    }
}

// Função para exibir a tela de pronto
void exibir_tela_pronto(void) { 
    limpar_tela();
    print_texto("SOUND", 1, 1, 2);
    print_texto("MONITOR", 20, 17, 2);
    print_texto("Pronto!", 25, 40, 2);
    timer_milliseconds(500); // Aguarda 0.5 segundo antes de continuar
}

