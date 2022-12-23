
#include "pico/time.h"
#include "ps1_memory_card.h"
#include "ps1_cardman.h"
#include "debug.h"

#include <gui.h>
#include <string.h>

static void clean_title_id(const char* const in_title_id, char* const out_title_id);


static void clean_title_id(const char* const in_title_id, char* const out_title_id)
{
    int idx_in_title = 0, idx_out_title = 0;

    while (in_title_id[idx_in_title] != 0x00)
    {
        if ((in_title_id[idx_in_title] == ';') || (in_title_id[idx_in_title] == 0x00)) {
            out_title_id[idx_out_title++] = 0x00;
            break;
        } else if ((in_title_id[idx_in_title] == '\\') || (in_title_id[idx_in_title] == '/')) {
            idx_out_title = 0;
        } else if (in_title_id[idx_in_title] == '_') {
            out_title_id[idx_out_title++] = '-';
        } else if (in_title_id[idx_in_title] != '.') {
            out_title_id[idx_out_title++] = in_title_id[idx_in_title];
        } else {
        }
        idx_in_title++;
    }
}

void ps1_odeman_init(void)
{

}

void ps1_odeman_task(void)
{
    uint8_t mc_pro_flags = ps1_memory_card_get_ode_flags();
    if (mc_pro_flags&MCP_GAME_ID)
    {
        char game_id[0xFF];
        char cleaned_game_id[0xFF] = { 0x00 };
        ps1_memory_card_get_game_id(game_id);
        debug_printf("PS1 Game ID Received: %s\n", game_id);
        clean_title_id(game_id, cleaned_game_id);
        debug_printf("Cleaned Game ID is %s\n", cleaned_game_id);
        ps1_memory_card_exit();
        ps1_cardman_close();
        ps1_cardman_set_gameid(cleaned_game_id);
        sleep_ms(100);
        ps1_cardman_open();
        ps1_memory_card_enter();
        gui_request_refresh();
    }
    if (mc_pro_flags&MCP_NXT_CH)
    {
        ps1_memory_card_exit();
        ps1_cardman_close();
        ps1_cardman_next_channel();
        sleep_ms(100);
        ps1_cardman_open();
        ps1_memory_card_enter();
        gui_request_refresh();
    }
    if (mc_pro_flags&MCP_PRV_CH)
    {
        ps1_memory_card_exit();
        ps1_cardman_close();
        ps1_cardman_prev_channel();
        sleep_ms(100);
        ps1_cardman_open();
        ps1_memory_card_enter();
        gui_request_refresh();
    }
    if (mc_pro_flags&MCP_NXT_CARD)
    {
        ps1_memory_card_exit();
        ps1_cardman_close();
        ps1_cardman_next_idx();
        sleep_ms(100);
        ps1_cardman_open();
        ps1_memory_card_enter();
        gui_request_refresh();
    }
    if (mc_pro_flags&MCP_PRV_CARD)
    {
        ps1_memory_card_exit();
        ps1_cardman_close();
        ps1_cardman_prev_idx();
        sleep_ms(100);
        ps1_cardman_open();
        ps1_memory_card_enter();
        gui_request_refresh();
    }
    ps1_memory_card_reset_ode_flags();
}