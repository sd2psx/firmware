#include <stdio.h>

#include "sd.h"
#include "debug.h"
#include "psram.h"

#include "hardware/timer.h"

#define CARD_SIZE (8 * 1024 * 1024)
#define BLOCK_SIZE (512)

static uint8_t flushbuf[BLOCK_SIZE];
static int fd;

void cardman_init(void) {
    fd = sd_open("CARD.BIN", O_RDWR);

    if (fd < 0) {
        fatal("cannot open card");
    }

    /* read 8 megs of card image */
    printf("reading card.... ");
    uint64_t start = time_us_64();
    for (size_t pos = 0; pos < CARD_SIZE; pos += BLOCK_SIZE) {
        if (sd_read(fd, flushbuf, BLOCK_SIZE) != BLOCK_SIZE)
            fatal("cannot read memcard");
        psram_write(pos, flushbuf, BLOCK_SIZE);
    }
    uint64_t end = time_us_64();
    printf("OK!\n");

    printf("took = %.2f s; SD read speed = %.2f kB/s\n", (end - start) / 1e6,
        1000000.0 * CARD_SIZE / (end - start) / 1024);
}

int cardman_write_sector(int sector, void *buf512) {
    if (fd < 0)
        return -1;
    
    if (sd_seek(fd, sector * BLOCK_SIZE) != 0)
        return -1;

    if (sd_write(fd, buf512, BLOCK_SIZE) != BLOCK_SIZE)
        return -1;
    
    return 0;
}

void cardman_flush(void) {
    if (fd >= 0)
        sd_flush(fd);
}
