#include "pico/stdlib.h"  // Biblioteca padrao do Raspberry Pi Pico
#include "lib/microfone.h"  // Biblioteca para leitura do microfone
#include "neopixel.c"  // Biblioteca para controle da matriz de LEDs NeoPixel
#include <string.h>  // Biblioteca para manipulacao de strings
#include <stdio.h>  // Biblioteca para entrada/saida padrao
#include "lib/wifi.h"  // Biblioteca para conexao Wi-Fi
#include "lib/display_oled.h"  // Biblioteca para controle do display OLED


// Variavel global para armazenar o nivel de decibels (dB)
float current_db_level = 0.0f;

// Definicao dos pinos dos botoes
#define BUTTON_A_PIN 5 // Botao para ligar o projeto
#define BUTTON_B_PIN 6 // Botao para desligar o projeto (Segurar ele para desligar)

// Variavel para controlar o estado do projeto (ligado/desligado)
bool projeto_ligado = false;


int main() {
    stdio_init_all();  // Inicializa a comunicacao serial via USB

    // Inicializa o LED onboard
    gpio_init(LED_PIN);
    // Aguarda 1 segundo
    timer_seconds(1);

    // Inicializa o LED onboard

    // Inicializa os botoes com pull-up interno ativado
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Inicializa a matriz de LEDs NeoPixel
    npInit(NEOPIXEL_PIN, LED_COUNT);
    
    // Inicializa o microfone
    microphone_init();

    // Inicializa os modulos necessarios
    inicializa();

    printf("Configuracoes completas!\n");
    printf("\n----\nAguardando botao A para iniciar...\n----\n");

    while (true) {
        // Verifica se o botao A foi pressionado para ligar o projeto
        if (!gpio_get(BUTTON_A_PIN) && !projeto_ligado) {
            // Menu de inicializacao com barra de progresso
            for (int i = 10; i <= 100; i += 10) {
                exibir_barra_carregamento(i);
                timer_milliseconds(200);  // Aguarda 200ms (0.2s)
            }
            exibir_tela_pronto(); // Exibe tela de pronto
            projeto_ligado = true;
            printf("\n[PROJECT] Projeto ligado!\n");

            // Tenta conectar ao Wi-Fi 5 vezes
            bool wifi_conectado = false;
            
                printf("[INFO] Tentativa de conexao ao Wi-Fi...\n");
                if (inicializar_wifi()) {
                    wifi_conectado = true;
                    printf("[INFO] Wi-Fi conectado com sucesso!\n");
                    
                }
                printf("[INFO] Falha na tentativa %d. Tentando novamente em 5 segundos...\n");
                timer_seconds(5);  // Aguarda 5 segundos antes de tentar novamente
            

            if (!wifi_conectado) {
                printf("[INFO] Nao foi possivel conectar ao Wi-Fi. O projeto continuara sem Wi-Fi.\n");
            }
        }

        // Verifica se o botao B foi pressionado para desligar o projeto
        if (!gpio_get(BUTTON_B_PIN) && projeto_ligado) {
            exibir_tela_desligar(); // Exibe a tela de desligamento
            
            projeto_ligado = false;
            printf("\n[PROJECT] Projeto desligado!\n");
            limpar_tela(); // Limpa o display OLED
            print_texto(" ", 1, 5, 2);
            print_texto(" ", 20, 20, 2);
            print_texto(" ", 5, 40, 1); 
            print_texto(" ", 5, 50, 1);
            limpar_matriz_led(); // Apaga a matriz de LEDs
            cyw43_arch_deinit(); // Desliga o Wi-Fi
            printf("[INFO] Wi-Fi desligado.\n");
        }

        // Se o projeto estiver ligado, executa o loop principal
        if (projeto_ligado) {
            cyw43_arch_poll();  // Mantem a conexao WiFi ativa (se houver)

            // Captura uma amostra do microfone e calcula a potencia media
            sample_mic();
            float avg = mic_power();
            avg = 2.f * fabsf(ADC_ADJUST(avg));

            // Aplica um filtro de media movel para suavizar as leituras
            float filtered_avg = apply_moving_average_filter(avg);

            // Converte a potencia filtrada para nivel de dB
            float db_level = calculate_db(filtered_avg);
            current_db_level = db_level; // Atualiza a variavel global

            // Classifica o volume baseado no nivel de dB
            const char* volume_level = classify_volume(db_level);

            // Atualiza os LEDs conforme o volume captado
            set_led_color_based_on_volume(db_level);

            // Exibe os valores de dB e classificacao no console
            printf("[DADOS] dB: %5.2f, Volume: %s\n\n", db_level, volume_level);

            // Prepara as strings para exibicao no display OLED
            char db_str[16];
            char volume_str[20]; 
            char *titulo1_str = "SOUND";
            char *titulo2_str = "MONITOR";

            sprintf(db_str, "dB: %5.2f", db_level);
            sprintf(volume_str, "Volume: %s", volume_level);

            // Exibe as informacoes no display OLED
            limpar_tela();
            print_texto(titulo1_str, 1, 5, 2);
            print_texto(titulo2_str, 20, 20, 2);
            print_texto(volume_str, 5, 40, 1);
            print_texto(db_str, 5, 50, 1);
            
            // Temporizador: Envia os dados ao ThingSpeak a cada 10 segundos (se o Wi-Fi estiver conectado)
            static absolute_time_t last_send_time = 0;
            if (wifi_connected && absolute_time_diff_us(last_send_time, get_absolute_time()) > 1000000) {
                send_data_to_thingspeak(db_level);
                last_send_time = get_absolute_time();
            }

            // Aguarda 1 segundo antes da proxima leitura
            timer_seconds(1);
        } else {
            // Se o projeto estiver desligado, aguarda um pouco antes de verificar novamente os botoes
            timer_milliseconds(100);
        }
    }

    cyw43_arch_deinit();  // Desliga o WiFi ao finalizar
    return 0;
}