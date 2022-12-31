
#include "pico/time.h"
#include "ps1_memory_card.h"
#include "ps1_cardman.h"
#include "debug.h"

#include <gui.h>
#include <string.h>

#define CARD_SWITCH_DELAY_MS    (120)


void ps1_odeman_init(void) {

}

void ps1_odeman_task(void) {
    uint8_t ode_command = ps1_memory_card_get_ode_command();

    if (ode_command != 0U) {
        ps1_memory_card_reset_ode_command();
        ps1_memory_card_exit();
        ps1_cardman_close();

        switch (ode_command) {
            case MCP_GAME_ID:
                printf("Received Game ID");
                const char *game_id;
                game_id = ps1_memory_card_get_game_id();
                printf("PS1 Game ID Received: %s\n", game_id);
                ps1_cardman_set_gameid(game_id);
                break;
            case MCP_NXT_CARD:
                ps1_cardman_next_idx();
                break;
            case MCP_PRV_CARD:
                ps1_cardman_prev_idx();
                break;
            case MCP_NXT_CH:
                ps1_cardman_next_channel();
                break;
            case MCP_PRV_CH:
                ps1_cardman_prev_channel();
                break;
            default:
                printf("Invalid ODE Command received.");
                break;
        }

        sleep_ms(CARD_SWITCH_DELAY_MS); // This delay is required, so ODE can register the card change

        ps1_cardman_open();
        ps1_memory_card_enter();
        gui_request_refresh();
    }

    /*

    if (mc_pro_flags&MCP_GAME_ID) {
        char game_id[0xFF];
        char cleaned_game_id[0x10] = { 0x00 };
        ps1_memory_card_get_game_id(game_id);
        debug_printf("PS1 Game ID Received: %s\n", game_id);
        clean_title_id(game_id, cleaned_game_id);
        debug_printf("Cleaned Game ID is %s\n", cleaned_game_id);
        ps1_memory_card_exit();
        ps1_cardman_close();
        ps1_cardman_set_gameid(cleaned_game_id);
        sleep_ms(CARD_SWITCH_DELAY_MS);
        ps1_cardman_open();
        ps1_memory_card_enter();
        gui_request_refresh();
    }
    if (mc_pro_flags&MCP_NXT_CH) {
        ps1_memory_card_exit();
        ps1_cardman_close();
        ps1_cardman_next_channel();
        sleep_ms(CARD_SWITCH_DELAY_MS);
        ps1_cardman_open();
        ps1_memory_card_enter();
        gui_request_refresh();
    }
    if (mc_pro_flags&MCP_PRV_CH) {
        ps1_memory_card_exit();
        ps1_cardman_close();
        ps1_cardman_prev_channel();
        sleep_ms(CARD_SWITCH_DELAY_MS);
        ps1_cardman_open();
        ps1_memory_card_enter();
        gui_request_refresh();
    }
    if (mc_pro_flags&MCP_NXT_CARD) {
        ps1_memory_card_exit();
        ps1_cardman_close();
        ps1_cardman_next_idx();
        sleep_ms(CARD_SWITCH_DELAY_MS);
        ps1_cardman_open();
        ps1_memory_card_enter();
        gui_request_refresh();
    }
    if (mc_pro_flags&MCP_PRV_CARD) {
        ps1_memory_card_exit();
        ps1_cardman_close();
        ps1_cardman_prev_idx();
        sleep_ms(CARD_SWITCH_DELAY_MS);
        ps1_cardman_open();
        ps1_memory_card_enter();
        gui_request_refresh();
    }
    */
}