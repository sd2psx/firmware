#pragma once


#include <stdint.h>

#define MCP_GAME_ID     (0b1U)
#define MCP_NXT_CH      (0b10U)
#define MCP_PRV_CH      (0b100U)
#define MCP_NXT_CARD    (0b1000U)
#define MCP_PRV_CARD    (0b10000U)


extern uint8_t mc_pro_flags;

void ps1_memory_card_main(void);
void ps1_memory_card_enter(void);
void ps1_memory_card_exit(void);

void ps1_memory_card_get_game_id(char* const game_id);