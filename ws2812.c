#include "ws2812.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"
#include <stdlib.h>

// Inicializa a fita/matriz WS2812
void ws2812_init(ws2812_t *ws, uint pin, uint num_leds) {
    ws->pin = pin;
    ws->num_leds = num_leds;
    ws->pixels = (uint8_t *)malloc(num_leds * 3); // 3 bytes por LED (RGB)

    // Configura o PIO para controlar os LEDs WS2812
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, pin, 800000);
}

// Define a cor de um LED específico
void ws2812_set_pixel(ws2812_t *ws, uint index, uint8_t r, uint8_t g, uint8_t b) {
    if (index < ws->num_leds) {
        ws->pixels[index * 3] = g;  // WS2812 usa ordem GRB
        ws->pixels[index * 3 + 1] = r;
        ws->pixels[index * 3 + 2] = b;
    }
}

// Apaga todos os LEDs
void ws2812_clear(ws2812_t *ws) {
    for (uint i = 0; i < ws->num_leds * 3; i++) {
        ws->pixels[i] = 0;
    }
}

// Atualiza os LEDs WS2812
void ws2812_show(ws2812_t *ws) {
    PIO pio = pio0;
    int sm = 0;

    for (uint i = 0; i < ws->num_leds; i++) {
        uint32_t pixel = ((uint32_t)ws->pixels[i * 3] << 16) | ((uint32_t)ws->pixels[i * 3 + 1] << 8) | ws->pixels[i * 3 + 2];
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }
}