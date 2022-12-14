#include "config.h"
#include "ssd1306.h"

#define blit16_ARRAY_ONLY
#define blit16_NO_HELPERS
#include "blit16.h"


static ssd1306_t oled_disp = { .external_vcc = 0 };
static int oled_init_done, have_oled;

int oled_init(void) {
    if (oled_init_done)
        return have_oled;
    oled_init_done = 1;

    i2c_init(OLED_I2C_PERIPH, OLED_I2C_CLOCK);
    gpio_set_function(OLED_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(OLED_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_I2C_SDA);
    gpio_pull_up(OLED_I2C_SCL);

    have_oled = ssd1306_init(&oled_disp, DISPLAY_WIDTH, DISPLAY_HEIGHT, OLED_I2C_ADDR, OLED_I2C_PERIPH);
    return have_oled;
}

void oled_clear(void) {
    ssd1306_clear(&oled_disp);
}

void oled_draw_pixel(int x, int y) {
    ssd1306_draw_pixel(&oled_disp, x, y);
}

void oled_show(void) {
    ssd1306_show(&oled_disp);
}

static int text_x, text_y;

static void draw_char(char c) {
    if (c == '\n') {
        text_x = 0;
        text_y += blit16_HEIGHT + 1;
        return;
    }

    if (c < 32 || c >= 32 + blit_NUM_GLYPHS)
        c = ' ';

    blit16_glyph g = blit16_Glyphs[c - 32];
    for (int y = 0; y < blit16_HEIGHT; ++y)
        for (int x = 0; x < blit16_WIDTH; ++x)
            if (g & (1 << (x + y * blit16_WIDTH)))
                oled_draw_pixel(text_x + x, text_y + y);

    text_x += blit16_WIDTH + 1;
    if (text_x >= DISPLAY_WIDTH) {
        text_x = 0;
        text_y += blit16_HEIGHT + 1;
    }
}

void oled_draw_text(const char *s) {
    for (; *s; s++)
        draw_char(*s);
}
