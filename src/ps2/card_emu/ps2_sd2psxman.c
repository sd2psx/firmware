#include <stdio.h>
#include "ps2/ps2_cardman.h"
#include "ps2/card_emu/ps2_memory_card.h"
#include "ps2/card_emu/ps2_sd2psxman.h"
#include "gui.h"
#include "debug.h"

#include "pico/time.h"
#include "ps2/card_emu/ps2_sd2psxman_commands.h"

volatile uint8_t sd2psxman_cmd;
volatile uint8_t sd2psxman_mode;
volatile uint16_t sd2psxman_cnum;
char sd2psxman_gameid[251] = { 0x00 };

void ps2_sd2psxman_task(void)
{
    if (sd2psxman_cmd != 0) {

        uint16_t prev_card = ps2_cardman_get_idx();
        uint8_t prev_chan = ps2_cardman_get_channel();
 
        switch (sd2psxman_cmd) {
            
            case SD2PSXMAN_SET_CARD:
                if (sd2psxman_mode == SD2PSXMAN_MODE_NUM) {
                    ps2_cardman_set_idx(sd2psxman_cnum);
                    debug_printf("set num idx\n");
                } else if (sd2psxman_mode == SD2PSXMAN_MODE_NEXT) {
                    ps2_cardman_next_idx();
                    debug_printf("set next idx\n");
                } else if (sd2psxman_mode == SD2PSXMAN_MODE_PREV) {
                    ps2_cardman_prev_idx();
                    debug_printf("set prev idx\n");
                }
                break;

            case SD2PSXMAN_SET_CHANNEL:
                if (sd2psxman_mode == SD2PSXMAN_MODE_NUM) {
                    ps2_cardman_set_channel(sd2psxman_cnum);
                    debug_printf("set num channel\n");
                } else if (sd2psxman_mode == SD2PSXMAN_MODE_NEXT) {
                    ps2_cardman_next_channel();
                    debug_printf("set next channel\n");
                } else if (sd2psxman_mode == SD2PSXMAN_MODE_PREV) {
                    ps2_cardman_prev_channel();
                    debug_printf("set prev channel\n");
                }
                break;
            
            case SD2PSXMAN_SET_GAMEID:
                //TODO:
                break;

            case SD2PSXMAN_UNMOUNT_BOOTCARD:
                if (ps2_cardman_get_idx() == 0) {
                    ps2_cardman_next_idx();
                }
                break;

            default:
            break;
        }

        if (prev_card != ps2_cardman_get_idx() || prev_chan != ps2_cardman_get_channel()) {
            //close old card
            ps2_memory_card_exit();
            ps2_cardman_close();

            //open new card
            ps2_cardman_open();
            ps2_memory_card_enter();
            gui_request_refresh();
        }

        sd2psxman_cmd = 0;
    }
}