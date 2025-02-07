#ifndef WS2812_H
#define WS2812_H

#include "pico/stdlib.h"

// Estrutura para armazenar informações da fita LED WS2812
typedef struct {
    uint pin;
    uint num_leds;
    uint8_t *pixels;
} ws2812_t;

// Inicializa a matriz WS2812
void ws2812_init(ws2812_t *ws, uint pin, uint num_leds);

// Define a cor de um LED específico (R, G, B)
void ws2812_set_pixel(ws2812_t *ws, uint index, uint8_t r, uint8_t g, uint8_t b);

// Apaga todos os LEDs
void ws2812_clear(ws2812_t *ws);

// Atualiza os LEDs para exibir as cores definidas
void ws2812_show(ws2812_t *ws);

#endif