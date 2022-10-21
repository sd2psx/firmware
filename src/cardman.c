#include "cardman.h"

#include <stdio.h>
#include <string.h>

#include "sd.h"
#include "debug.h"
#include "psram.h"

#include "hardware/timer.h"

#define CARD_SIZE (8 * 1024 * 1024)
#define BLOCK_SIZE (512)

static uint8_t flushbuf[BLOCK_SIZE];
static int fd = -1;

#define IDX_MIN 1
#define CHAN_MIN 1
#define CHAN_MAX 8

static int card_idx;
static int card_chan;

void cardman_init(void) {
    // TODO: should load last used card from eeprom
    card_idx = card_chan = 1;
    cardman_open();
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

static void ensuredirs(void) {
    char cardpath[32];
    snprintf(cardpath, sizeof(cardpath), "MemoryCards/PS2/Card%d", card_idx);

    sd_mkdir("MemoryCards");
    sd_mkdir("MemoryCards/PS2");
    sd_mkdir(cardpath);

    if (!sd_exists("MemoryCards") || !sd_exists("MemoryCards/PS2") || !sd_exists(cardpath))
        fatal("error creating directories");
}

static void make_empty_img(const char *path) {
    uint64_t start = time_us_64();
    printf("create new image at %s... ", path);

    if (sd_exists(path))
        fatal("refuse to recreate existing path");

    int fd = sd_open(path, O_RDWR | O_CREAT | O_TRUNC);
    if (fd < 0)
        fatal("cannot open for creating new card");

    memset(flushbuf, 0xFF, sizeof(flushbuf));
    for (size_t pos = 0; pos < CARD_SIZE; pos += BLOCK_SIZE) {
        if (sd_write(fd, flushbuf, BLOCK_SIZE) != BLOCK_SIZE)
            fatal("cannot init memcard");
    }

    sd_close(fd);

    uint64_t end = time_us_64();
    printf("OK!\n");

    printf("took = %.2f s; SD write speed = %.2f kB/s\n", (end - start) / 1e6,
        1000000.0 * CARD_SIZE / (end - start) / 1024);
}

void cardman_open(void) {
    char path[64];

    ensuredirs();
    snprintf(path, sizeof(path), "MemoryCards/PS2/Card%d/Card%d-%d.mcd", card_idx, card_idx, card_chan);

    printf("Switching to card path = %s\n", path);

    // TODO: should special case this for faster ops otherwise it's 2x slower than reusing a card
    if (!sd_exists(path))
        make_empty_img(path);

    fd = sd_open(path, O_RDWR);

    if (fd < 0)
        fatal("cannot open card");

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

void cardman_close(void) {
    if (fd < 0)
        return;
    cardman_flush();
    sd_close(fd);
    fd = -1;
}

void cardman_next_channel(void) {
    card_chan += 1;
    if (card_chan > CHAN_MAX)
        card_chan = CHAN_MIN;
}

void cardman_prev_channel(void) {
    card_chan -= 1;
    if (card_chan < CHAN_MIN)
        card_chan = CHAN_MAX;
}

void cardman_next_idx(void) {
    card_idx += 1;
    card_chan = 1;
}

void cardman_prev_idx(void) {
    card_idx -= 1;
    card_chan = 1;
    if (card_idx < IDX_MIN)
        card_idx = IDX_MIN;
}

int cardman_get_idx(void) {
    return card_idx;
}

int cardman_get_channel(void) {
    return card_chan;
}
