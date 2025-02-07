#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "font.h"
#include "ws2812.h"  // Biblioteca para os LEDs WS2812
#include "ssd1306.h" // Biblioteca para o display OLED

// Definições de hardware
#define I2C_PORT i2c0
#define SDA_PIN 14
#define SCL_PIN 15
#define UART_ID uart0
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define LED_PIN 7
#define NUM_LEDS 25

// Definições dos botões e LEDs RGB
#define BUTTON_A 5
#define BUTTON_B 6
#define LED_GREEN 12
#define LED_BLUE 13
#define DEBOUNCE_TIME 200 // Tempo de debounce em milissegundos

// Estado dos LEDs
volatile bool led_green_state = false;
volatile bool led_blue_state = false;

// Último tempo de acionamento para debounce
volatile uint32_t last_press_A = 0;
volatile uint32_t last_press_B = 0;

// Inicializa o display OLED SSD1306
ssd1306_t display;

// Inicializa a matriz de LEDs WS2812
ws2812_t matriz;

// Mapeamento dos números (5x5)
const uint8_t numeros[10][5] = {
    {0b01110, 0b10001, 0b10001, 0b10001, 0b01110}, // 0
    {0b00100, 0b01100, 0b00100, 0b00100, 0b01110}, // 1
    {0b01110, 0b00001, 0b01110, 0b10000, 0b11111}, // 2
    {0b01110, 0b00001, 0b00110, 0b00001, 0b01110}, // 3
    {0b00010, 0b00110, 0b01010, 0b11111, 0b00010}, // 4
    {0b11111, 0b10000, 0b11110, 0b00001, 0b11110}, // 5
    {0b01111, 0b10000, 0b11110, 0b10001, 0b01110}, // 6
    {0b11111, 0b00001, 0b00010, 0b00100, 0b01000}, // 7
    {0b01110, 0b10001, 0b01110, 0b10001, 0b01110}, // 8
    {0b01110, 0b10001, 0b01111, 0b00001, 0b01110}  // 9
};

// Exibe um caractere no display SSD1306
void exibir_caractere_display(char c) {
    ssd1306_clear(&display);
    ssd1306_draw_char(&display, c, 40, 20);
    ssd1306_update(&display);
}

// Exibe um número (0-9) na matriz WS2812
void exibir_numero_matriz(int num) {
    ws2812_clear(&matriz);
    for (int linha = 0; linha < 5; linha++) {
        for (int coluna = 0; coluna < 5; coluna++) {
            int index = linha * 5 + coluna;
            if (numeros[num][linha] & (1 << (4 - coluna))) {
                ws2812_set_pixel(&matriz, index, 0, 255, 0);  // Verde
            } else {
                ws2812_set_pixel(&matriz, index, 0, 0, 0);  // Apagado
            }
        }
    }
    ws2812_show(&matriz);
}

// Função para exibir mensagens no display
void exibir_mensagem_display(const char *mensagem) {
    ssd1306_clear(&display);
    ssd1306_draw_string(&display, mensagem, 10, 20);
    ssd1306_update(&display);
}

// Interrupção do botão A (LED Verde) com Debouncing
void botaoA_irq_handler(uint gpio, uint32_t events) {
    uint32_t agora = to_ms_since_boot(get_absolute_time());
    if (agora - last_press_A < DEBOUNCE_TIME) return; // Ignora bouncing
    last_press_A = agora;

    led_green_state = !led_green_state;
    gpio_put(LED_GREEN, led_green_state);

    printf("Botão A pressionado! LED Verde: %s\n", led_green_state ? "Ligado" : "Desligado");
    exibir_mensagem_display(led_green_state ? "LED Verde Ligado" : "LED Verde Desligado");
}

// Interrupção do botão B (LED Azul) com Debouncing
void botaoB_irq_handler(uint gpio, uint32_t events) {
    uint32_t agora = to_ms_since_boot(get_absolute_time());
    if (agora - last_press_B < DEBOUNCE_TIME) return; // Ignora bouncing
    last_press_B = agora;

    led_blue_state = !led_blue_state;
    gpio_put(LED_BLUE, led_blue_state);

    printf("Botão B pressionado! LED Azul: %s\n", led_blue_state ? "Ligado" : "Desligado");
    exibir_mensagem_display(led_blue_state ? "LED Azul Ligado" : "LED Azul Desligado");
}

int main() {
    // Inicializa a UART
    uart_init(UART_ID, 115200);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Inicializa o I2C para o display SSD1306
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    
    // Inicializa o display
    ssd1306_init(&display, 128, 64, false, 0x3C, I2C_PORT);
    ssd1306_clear(&display);
    ssd1306_update(&display);

    // Inicializa a matriz WS2812
    ws2812_init(&matriz, LED_PIN, NUM_LEDS);
    ws2812_clear(&matriz);
    ws2812_show(&matriz);

    // Inicializa os LEDs RGB como saída
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, false);

    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_put(LED_BLUE, false);

    // Configuração dos botões como entrada com pull-up
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    // Configuração das interrupções
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &botaoA_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &botaoB_irq_handler);

    // Mensagem inicial
    printf("Sistema iniciado. Digite um caractere...\n");

    while (1) {
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            printf("Recebido: %c\n", c);

            // Exibir no display OLED
            exibir_caractere_display(c);

            // Se for número, exibir na matriz de LEDs
            if (c >= '0' && c <= '9') {
                int num = c - '0';  // Converte char para int
                exibir_numero_matriz(num);
            }
        }
    }
}