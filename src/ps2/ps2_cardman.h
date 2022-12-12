#pragma once

void ps2_cardman_init(void);
int ps2_cardman_write_sector(int sector, void *buf512);
void ps2_cardman_flush(void);
void ps2_cardman_open(void);
void ps2_cardman_close(void);
int ps2_cardman_get_idx(void);
int ps2_cardman_get_channel(void);

void ps2_cardman_next_channel(void);
void ps2_cardman_prev_channel(void);
void ps2_cardman_next_idx(void);
void ps2_cardman_prev_idx(void);

typedef void (*cardman_cb_t)(int);

void ps2_cardman_set_progress_cb(cardman_cb_t func);
char *ps2_cardman_get_progress_text(void);
