#pragma once

#include <stdint.h>

extern volatile uint8_t sd2psxman_cmd;
extern volatile uint8_t sd2psxman_mode;
extern volatile uint16_t sd2psxman_cnum;
extern char sd2psxman_gameid[251];

void ps2_sd2psxman_task(void);