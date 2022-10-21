#pragma once

void cardman_init(void);
int cardman_write_sector(int sector, void *buf512);
void cardman_flush(void);
void cardman_open(void);
void cardman_close(void);
int cardman_get_idx(void);
int cardman_get_channel(void);

void cardman_next_channel(void);
void cardman_prev_channel(void);
void cardman_next_idx(void);
void cardman_prev_idx(void);
