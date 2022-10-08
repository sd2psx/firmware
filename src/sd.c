#include <stdio.h>
#include <inttypes.h>

#include "pico/stdlib.h"

#define CYCLES 1000

// FIL f_memcard;

void sd_init(void) {
    printf("SD init!\n");
#if 0

    sd_card_t *sd = sd_get_by_num(0);
    FRESULT ret = f_mount(&sd->fatfs, sd->pcName, 1);
    if (ret != FR_OK)
        panic("f_mount error: %s (%d)\n", FRESULT_str(ret), ret);

    // FIL fp;
    ret = f_open(&f_memcard, "CARD.BIN", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    if (ret != FR_OK)
        fatal("cannot open");

    if (f_size(&f_memcard) != 8 * 1024 * 1024) {
        printf("creating new card!! because old size = %d\n", f_size(&f_memcard));
        if (f_truncate(&f_memcard) != FR_OK)
            fatal("cannot truncate");
        if (f_expand(&f_memcard, 8 * 1024 * 1024, 1) != FR_OK)
            fatal("cannot make memcard");
    } else {
        printf("card OK - reusing it\n");
    }
    f_sync(&f_memcard);

    /* now read the memcard into psram */
    printf("reading memcard... ");
    uint64_t start = time_us_64();
    static uint8_t buf[4096];
    for (int pos = 0; pos < 8 * 1024 * 1024; pos += sizeof(buf)) {
        UINT read = 0;
        ret = f_read(&f_memcard, buf, sizeof(buf), &read);
        if (ret != FR_OK || read != sizeof(buf))
            fatal("cannot read");
        // for (int i = 0; i < 4096; i += 512)
        //     psram_write(pos, buf + i, 512);
    }
    uint64_t end = time_us_64();
    printf("OK! - took %d ms - %.2f kB/s\n", (int)((end - start) / 1000), (8.0 * 1024 * 1000000) / (end - start));
#endif
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
