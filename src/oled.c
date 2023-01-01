#include "config.h"
#include "ssd1306.h"


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
