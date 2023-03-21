/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// from Pi4-02

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define IS_RGBW     false

#define NUM_PIXELS  (7 + 12)
#define delay_ms    20

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
// default to pin 2 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN 29
#endif

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) |
           ((uint32_t)(g) << 16) |
           (uint32_t)(b);
}

long counter = 0;

void pattern_snakes(uint len, uint t)
{
    //char colors[4][10] = {"Red  ", "Green", "Blue ", "Off  "};
    //char* color;
    //printf("% 12d\n", counter);
    for (uint i = 0; i < len; ++i)
    {
        uint x = (i + (t >> 1)) % 64;
        if (x < 10)
        {   
            //color = "Red  ";
            put_pixel(urgb_u32(0x07, 0, 0));
        }
        else if (x >= 15 && x < 25)
        {
            //color = "Green";
            put_pixel(urgb_u32(0, 0x07, 0));
        }
        else if (x >= 30 && x < 40)
        {
            //color = "Blue ";
            put_pixel(urgb_u32(0, 0, 0x1f));
        }
        else
        {
            //color = "Off  ";
            put_pixel(0);
        }
        //printf("             - %s - % 2d\n", color, (t >> 1));
    }
}

void pattern_random(uint len, uint t)
{
    if (t % 8)
        return;
    for (int i = 0; i < len; ++i)
        put_pixel(rand());
}

void pattern_sparkle(uint len, uint t)
{
    if (t % 8)
        return;
    for (int i = 0; i < len; ++i)
        put_pixel(rand() % 16 ? 0 : 0xffffffff);
}

void pattern_greys(uint len, uint t)
{
    int max = 100; // let's not draw too much current!
    t %= max;
    for (int i = 0; i < len; ++i)
    {
        put_pixel(t * 0x10101);
        if (++t >= max)
            t = 0;
    }
}

typedef void (*pattern)(uint len, uint t);
const struct
{
    pattern pat;
    const char *name;
} pattern_table[] = {
    {pattern_snakes,    "Snakes!    "},
    {pattern_random,    "Random data"},
    {pattern_sparkle,   "Sparkles   "},
    {pattern_greys,     "Greys      "},
};

int main()
{
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // set_sys_clock_48();
    stdio_init_all();
    printf("WS2812 Smoke Test, using pin %d", WS2812_PIN);

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    int t = 0;
    // long counter = 0;
    int toggle = 0;
    while (1)
    {
        int pat = rand() % count_of(pattern_table);
        int dir = (rand() >> 30) & 1 ? 1 : -1;
        printf("% 12s - ", pattern_table[pat].name);
        puts(dir == 1 ? "(forward)" : "(backward)");
        for (int i = 0; i < 1000; ++i)
        {
            printf("% 12d\r", counter++);
            pattern_table[pat].pat(NUM_PIXELS, t);
            gpio_put(LED_PIN, (toggle ^= 1));
            sleep_ms(delay_ms);
            t += dir;
        }
    }
}
