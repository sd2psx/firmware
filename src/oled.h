#pragma once

int oled_init(void);
void oled_clear(void);
void oled_draw_pixel(int x, int y);
void oled_show(void);
void oled_draw_text(const char *s);
