#pragma once

void cardman_init(void);
int cardman_write_sector(int sector, void *buf512);
void cardman_flush(void);
