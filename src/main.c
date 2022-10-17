#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "hardware/structs/bus_ctrl.h"

#include "memory_card.h"
#include "gui.h"
#include "input.h"
#include "config.h"
#include "debug.h"
#include "psram.h"
#include "sd.h"
#include "dirty.h"

/* reboot to bootloader if either button is held on startup
   to make the device easier to flash when assembled inside case */
static void check_bootloader_reset(void) {
    /* make sure at least DEBOUNCE interval passes or we won't get inputs */
    for (int i = 0; i < 2 * DEBOUNCE_MS; ++i) {
        input_task();
        sleep_ms(1);
    }

    if (input_is_down(0) || input_is_down(1))
        reset_usb_boot(0, 0);
}

static void debug_task(void) {
    for (int i = 0; i < 10; ++i) {
        char ch = debug_get();
        if (ch) {
            if (ch == '\n')
                uart_putc_raw(UART_PERIPH, '\r');
            uart_putc_raw(UART_PERIPH, ch);
        } else {
            break;
        }
    }
}

int main() {
    input_init();
    check_bootloader_reset();

    stdio_uart_init_full(UART_PERIPH, UART_BAUD, UART_TX, UART_RX);

    printf("prepare...\n");
    int mhz = 240;
    set_sys_clock_khz(mhz * 1000, true);
    clock_configure(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS, mhz * 1000000, mhz * 1000000);

    stdio_uart_init_full(UART_PERIPH, UART_BAUD, UART_TX, UART_RX);

    /* set up core1 as high priority bus access */
    bus_ctrl_hw->priority |= BUSCTRL_BUS_PRIORITY_PROC1_BITS;
    while (!bus_ctrl_hw->priority_ack) {}

    printf("\n\n\nStarted! Clock %d; bus priority 0x%X\n", (int)clock_get_hz(clk_sys), (unsigned)bus_ctrl_hw->priority);

    psram_init();
    sd_init();
    dirty_init();
    gui_init();

    multicore_launch_core1(memory_card_main);

    printf("Starting memory card... ");
    uint64_t start = time_us_64();
    memory_card_enter();
    uint64_t end = time_us_64();
    printf("DONE! (%d us)\n", (int)(end - start));

    while (1) {
        debug_task();
        dirty_task();
        gui_task();
        input_task();
    }
}
