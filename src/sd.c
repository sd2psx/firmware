#include <stdio.h>

#include "f_util.h"
#include "ff.h"
#include "pico/stdlib.h"
#include "rtc.h"

#include "hw_config.h"

char buf[4096];

#define CYCLES 1000

void sd_init(void) {
    printf("SD init!\n");

    sd_card_t *sd = sd_get_by_num(0);
    FRESULT ret = f_mount(&sd->fatfs, sd->pcName, 1);
    if (ret != FR_OK)
        panic("f_mount error: %s (%d)\n", FRESULT_str(ret), ret);

    // FIL fp;
    // ret = f_open(&fp, "kernel8.img", FA_READ);
    // if (ret != FR_OK)
    //     fatal("cannot open");

    // uint32_t total = 0;

    // uint64_t start = time_us_64();

    // for (int i = 0; i < CYCLES; ++i) {
    //     sd_read_blocks(sd, buf, 128 + i * 1000, sizeof(buf) / 512);
    //     total += sizeof(buf);
    // }

    // while (1) {
    //     UINT read = 0;
    //     ret = f_read(&fp, buf, sizeof(buf), &read);
    //     if (ret != FR_OK)
    //         fatal("cannot read");
    //     total += read;
    //     if (read < sizeof(buf))
    //         break;
    // }

    // uint64_t end = time_us_64();
    // printf("FS read -- took %.2f ms -- avg speed %.2f kB/s\n",
    //     (end - start) / 1000.0,
    //     1000000.0 * (total) / (end - start) / 1024);
}
