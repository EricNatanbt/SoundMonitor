// Inclusão de bibliotecas necessárias
#include "pico/cyw43_arch.h"  // Biblioteca para gerenciar o chip Wi-Fi CYW43 no Raspberry Pi Pico W
#include "lwip/tcp.h"         // Biblioteca para gerenciar conexões TCP (parte do lwIP)
#include "lwip/dns.h"         // Biblioteca para resolução de nomes DNS (parte do lwIP)
#include "lib/wifi.h"         // Possivelmente uma biblioteca personalizada para gerenciar Wi-Fi

// Configurações do Wi-Fi
#define WIFI_SSID "HOTSPOTNOTEBOOK"  // Nome da rede Wi-Fi (substitua pelo seu SSID)
#define WIFI_PASS "eric2810"         // Senha da rede Wi-Fi (substitua pela sua senha)

// Configurações do ThingSpeak
#define THINGSPEAK_HOST "api.thingspeak.com"  // Endereço do servidor ThingSpeak
#define THINGSPEAK_PORT 80                    // Porta HTTP para comunicação
#define API_KEY "GZ8Y76BXEDG4FVDA"            // Chave de API do ThingSpeak (substitua pela sua)

// Variável para controlar o estado da conexão Wi-Fi
bool wifi_connected = false;

// Variável externa para armazenar o valor global de decibéis (dB)
extern float current_db_level;

// Variáveis para controle de temporização
static uint32_t last_check_time = 0;              // Armazena o tempo da última verificação
static const uint32_t CHECK_INTERVAL_MS = 10000;  // Intervalo de verificação (10 segundos)


// Função de temporizador em segundos
void timer_seconds(int seconds) {
    uint64_t start_time = time_us_64(); // Obtém o tempo atual em microssegundos
    uint64_t delay = seconds * 1000000; // Converte segundos para microssegundos

    // Aguarda até que o tempo especificado tenha passado
    while (time_us_64() - start_time < delay) {
        // Loop vazio (busy-wait)
    }
}

/**
 * Verifica se é hora de realizar uma nova verificação.
 * Retorna true se o intervalo de tempo (CHECK_INTERVAL_MS) foi atingido.
 */
static bool should_check() {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());  // Obtém o tempo atual
    if (current_time - last_check_time >= CHECK_INTERVAL_MS) {      // Verifica o intervalo
        last_check_time = current_time;  // Atualiza o tempo da última verificação
        return true;
    }
    return false;
}

/**
 * Temporizador: Funcao para inicializar o WiFi com tentativas repetidas.
 */
bool inicializar_wifi() {
    if (cyw43_arch_init()) {
        printf("[ERRO] Erro ao inicializar o WiFi\n");
        return false;
    }
    cyw43_arch_enable_sta_mode();

    if (!connect_to_wifi()) {
        printf("[ERRO] Falha ao conectar ao WiFi.\n");
        return false;
    }

    printf("[INFO] WiFi conectado com sucesso!\n");
    return true;
}


/**
 * Callback chamado quando os dados são enviados com sucesso ao servidor.
 */
static err_t tcp_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    printf("[DADOS] Dados enviados com sucesso!\n");
    tcp_close(tpcb);  // Fecha a conexão TCP após o envio
    return ERR_OK;
}

/**
 * Callback chamado quando a conexão TCP com o servidor é estabelecida.
 */
static err_t tcp_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err == ERR_OK) {
        printf("[INFO] Conectado ao servidor!\n");

        // Prepara a requisição HTTP GET com o valor de dB atual
        float db_level = current_db_level;
        char request[200];
        snprintf(request, sizeof(request),
                 "GET /update?api_key=%s&field1=%.2f HTTP/1.1\r\n"
                 "Host: api.thingspeak.com\r\n"
                 "Connection: close\r\n\r\n",
                 API_KEY, db_level);

        // Envia a requisição ao servidor
        tcp_write(tpcb, request, strlen(request), TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);  // Força o envio dos dados
        tcp_sent(tpcb, tcp_sent_callback);  // Configura o callback para pós-envio
    } else {
        printf("[ERRO] Falha ao conectar ao servidor\n");
    }
    return ERR_OK;
}

/**
 * Callback chamado quando a resolução DNS é concluída.
 */
static void dns_resolve_callback(const char *name, const ip_addr_t *ipaddr, void *arg) {
    if (ipaddr != NULL) {
        printf("[INFO] Endereço IP do ThingSpeak resolvido: %s\n", ipaddr_ntoa(ipaddr));

        // Cria uma nova conexão TCP
        struct tcp_pcb *pcb = tcp_new();
        if (!pcb) {
            printf("[ERRO] Erro ao criar conexão TCP\n");
            return;
        }

        // Tenta conectar ao servidor ThingSpeak
        tcp_connect(pcb, ipaddr, THINGSPEAK_PORT, tcp_connected_callback);
    } else {
        printf("[ERRO] Erro na resolução de DNS\n");
    }
}

/**
 * Envia os dados de dB para o ThingSpeak.
 */
bool send_data_to_thingspeak(float db_level) {
    if (!wifi_connected) {
        printf("[INFO] WiFi desconectado. Não é possível enviar dados.\n");
        return false;
    }

    // Inicia a resolução DNS do host do ThingSpeak
    ip_addr_t server_ip;
    cyw43_arch_lwip_begin();
    err_t err = dns_gethostbyname(THINGSPEAK_HOST, &server_ip, dns_resolve_callback, NULL);
    cyw43_arch_lwip_end();

    if (err != ERR_OK) {
        return true;
    }

    printf("[INFO] Iniciando envio de dados para o ThingSpeak...\n");
}

/**
 * Conecta ao Wi-Fi usando as credenciais fornecidas.
 */
bool connect_to_wifi() {
    printf("[INFO] Conectando ao WiFi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("[ERRO] Erro ao conectar ao WiFi\n");
        wifi_connected = false;
        return false;
    }
    printf("[INFO] Conectado ao WiFi!\n");
    printf("Endereço IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
    wifi_connected = true;
    return true;
}

/**
 * Função principal de monitoramento.
 * Executa um loop infinito para verificar e enviar dados periodicamente.
 */
void monitor() {
    while (true) {
        if (should_check()) {  // Verifica se é hora de enviar dados
            // Reconecta ao Wi-Fi se necessário
            if (!wifi_connected) {
                connect_to_wifi();
            }

            // Envia os dados de dB ao ThingSpeak
            send_data_to_thingspeak(current_db_level);
        }

        // Aguarda 1 segundo antes de verificar novamente
        timer_seconds(1);
    }
}

