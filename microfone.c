#include "lib/microfone.h"  // Inclui o cabeçalho com definições e constantes específicas do microfone

// Variáveis globais
uint dma_channel;                     // Canal DMA usado para transferir dados do ADC
dma_channel_config dma_cfg;           // Configuração do canal DMA
uint16_t adc_buffer[SAMPLES];         // Buffer para armazenar as amostras do ADC
float filter_buffer[FILTER_SIZE] = {0}; // Buffer para o filtro de média móvel
uint filter_index = 0;                // Índice atual do buffer do filtro

/**
 * Inicializa o microfone e o ADC.
 */
void microphone_init() {
    // Configuração do ADC
    adc_gpio_init(MIC_PIN);           // Inicializa o pino GPIO conectado ao microfone
    adc_init();                       // Inicializa o ADC
    adc_select_input(MIC_CHANNEL);    // Seleciona o canal do ADC para o microfone

    // Configura o FIFO do ADC
    adc_fifo_setup(
        true,  // Habilitar FIFO
        true,  // Habilitar request de dados do DMA
        1,     // Threshold para ativar request DMA é 1 leitura do ADC
        false, // Não usar bit de erro
        false  // Manter amostras em 12 bits (não fazer downscale para 8 bits)
    );

    adc_set_clkdiv(ADC_CLOCK_DIV);    // Configura o divisor de clock do ADC

    // Configuração do DMA
    dma_channel = dma_claim_unused_channel(true);  // Obtém um canal DMA livre
    dma_cfg = dma_channel_get_default_config(dma_channel);  // Obtém a configuração padrão do DMA
    channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16);  // Define o tamanho dos dados como 16 bits
    channel_config_set_read_increment(&dma_cfg, false);  // Não incrementar o endereço de leitura (lê sempre do FIFO do ADC)
    channel_config_set_write_increment(&dma_cfg, true);   // Incrementar o endereço de escrita (escreve no buffer)
    channel_config_set_dreq(&dma_cfg, DREQ_ADC);         // Usa o ADC como fonte de dados para o DMA
}

/**
 * Realiza as leituras do ADC e armazena os valores no buffer.
 */
void sample_mic() {
    adc_fifo_drain();  // Limpa o FIFO do ADC para evitar dados antigos
    adc_run(false);    // Desliga o ADC (se estiver ligado) para configurar o DMA

    // Configura o DMA para transferir dados do FIFO do ADC para o buffer
    dma_channel_configure(dma_channel, &dma_cfg,
        adc_buffer,    // Escreve no buffer de amostras
        &(adc_hw->fifo), // Lê do FIFO do ADC
        SAMPLES,       // Número de amostras a serem coletadas
        true           // Inicia o DMA imediatamente
    );

    // Liga o ADC e espera o DMA terminar a transferência
    adc_run(true);
    dma_channel_wait_for_finish_blocking(dma_channel);

    // Desliga o ADC após a leitura
    adc_run(false);
}

/**
 * Calcula a potência média das leituras do ADC (valor RMS).
 */
float mic_power() {
    float avg = 0.f;

    // Soma os quadrados das amostras para calcular a potência
    for (uint i = 0; i < SAMPLES; ++i)
        avg += adc_buffer[i] * adc_buffer[i];
    
    avg /= SAMPLES;  // Calcula a média
    return sqrt(avg); // Retorna a raiz quadrada (valor RMS)
}

/**
 * Aplica um filtro de média móvel para suavizar as leituras.
 */
float apply_moving_average_filter(float new_value) {
    filter_buffer[filter_index] = new_value;  // Adiciona o novo valor ao buffer do filtro
    filter_index = (filter_index + 1) % FILTER_SIZE;  // Atualiza o índice do buffer (circular)

    // Calcula a média dos valores no buffer
    float sum = 0.f;
    for (uint i = 0; i < FILTER_SIZE; ++i) {
        sum += filter_buffer[i];
    }
    return sum / FILTER_SIZE;  // Retorna a média
}

/**
 * Calcula o nível de dB a partir da tensão.
 */
float calculate_db(float voltage) {
    float reference_voltage = 0.0001f;  // Tensão de referência para 0 dB (ajuste conforme necessário)
    float db = 20.0f * log10f(voltage / reference_voltage);  // Calcula o nível de dB
    return db;
}

/**
 * Classifica o volume com base no nível de dB.
 */
const char* classify_volume(float db) {
    if (db < 30) {
        return "Muito Baixo";
    } else if (db < 43) {
        return "Baixo";
    } else if (db < 55) {
        return "Moderado";
    } else if (db < 60) {
        return "Alto";
    } else if (db < 90) {
        return "Muito Alto";
    } else {
        return "Extremamente Alto";
    }
}