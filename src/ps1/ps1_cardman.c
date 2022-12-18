#include "ps1_cardman.h"

#include <stdio.h>
#include <string.h>

#include "sd.h"
#include "debug.h"
#include "settings.h"
#include "bigmem.h"
#include "ps1_empty_card.h"

#include "hardware/timer.h"

#define CARD_SIZE (128 * 1024)
#define BLOCK_SIZE 128
static uint8_t flushbuf[BLOCK_SIZE];
static int fd = -1;

#define IDX_MIN 1
#define IDX_GAMEID 0
#define CHAN_MIN 1
#define CHAN_MAX 8

static int card_idx;
static int card_chan;
static char card_game_id[0xFF];

void ps1_cardman_init(void) {
    card_idx = settings_get_ps1_card();
    if (card_idx < IDX_MIN)
        card_idx = IDX_MIN;
    card_chan = settings_get_ps1_channel();
    if (card_chan < CHAN_MIN || card_chan > CHAN_MAX)
        card_chan = CHAN_MIN;
    memset(card_game_id, 0, 0xFF);
}

int ps1_cardman_write_sector(int sector, void *buf512) {
    if (fd < 0)
        return -1;

    if (sd_seek(fd, sector * BLOCK_SIZE) != 0)
        return -1;

    if (sd_write(fd, buf512, BLOCK_SIZE) != BLOCK_SIZE)
        return -1;

    return 0;
}

void ps1_cardman_flush(void) {
    if (fd >= 0)
        sd_flush(fd);
}

static void ensuredirs(void) {
    char cardpath[32];
    if (card_game_id[0] != 0x00) {
        snprintf(cardpath, sizeof(cardpath), "MemoryCards/PS1/%s", card_game_id);
    } else {
        snprintf(cardpath, sizeof(cardpath), "MemoryCards/PS1/Card%d", card_idx);
    }

    sd_mkdir("MemoryCards");
    sd_mkdir("MemoryCards/PS1");
    sd_mkdir(cardpath);

    if (!sd_exists("MemoryCards") || !sd_exists("MemoryCards/PS1") || !sd_exists(cardpath))
        fatal("error creating directories");
}

static void genblock(size_t pos, void *buf) {
    memset(buf, 0xFF, BLOCK_SIZE);

    if (pos < 0x2000)
        memcpy(buf, &ps1_empty_card[pos], BLOCK_SIZE);
}

void ps1_cardman_open(void) {
    char path[64];

    ensuredirs();
    if ((card_game_id[0] != 0x00) && (card_idx < IDX_MIN)) {
        snprintf(path, sizeof(path), "MemoryCards/PS1/%s/%s-%d.mcd", card_game_id, card_game_id, card_chan);
    } else {
        snprintf(path, sizeof(path), "MemoryCards/PS1/Card%d/Card%d-%d.mcd", card_idx, card_idx, card_chan);
        /* this is ok to do on every boot because it wouldn't update if the value is the same as currently stored */
        settings_set_ps1_card(card_idx);
        settings_set_ps1_channel(card_chan);
    }

    printf("Switching to card path = %s\n", path);
    
    if (!sd_exists(path)) {
        fd = sd_open(path, O_RDWR | O_CREAT | O_TRUNC);

        if (fd < 0)
            fatal("cannot open for creating new card");

        printf("create new image at %s... ", path);
        uint64_t cardprog_start = time_us_64();

        for (size_t pos = 0; pos < CARD_SIZE; pos += BLOCK_SIZE) {
            genblock(pos, flushbuf);
            if (sd_write(fd, flushbuf, BLOCK_SIZE) != BLOCK_SIZE)
                fatal("cannot init memcard");
            memcpy(&bigmem.ps1.card_image[pos], flushbuf, BLOCK_SIZE);
        }
        sd_flush(fd);

        uint64_t end = time_us_64();
        printf("OK!\n");

        printf("took = %.2f s; SD write speed = %.2f kB/s\n", (end - cardprog_start) / 1e6,
            1000000.0 * CARD_SIZE / (end - cardprog_start) / 1024);
    } else {
        fd = sd_open(path, O_RDWR);

        if (fd < 0)
            fatal("cannot open card");

        /* read 8 megs of card image */
        printf("reading card.... ");
        uint64_t cardprog_start = time_us_64();
        for (size_t pos = 0; pos < CARD_SIZE; pos += BLOCK_SIZE) {
            if (sd_read(fd, flushbuf, BLOCK_SIZE) != BLOCK_SIZE)
                fatal("cannot read memcard");
            memcpy(&bigmem.ps1.card_image[pos], flushbuf, BLOCK_SIZE);
        }
        uint64_t end = time_us_64();
        printf("OK!\n");

        printf("took = %.2f s; SD read speed = %.2f kB/s\n", (end - cardprog_start) / 1e6,
            1000000.0 * CARD_SIZE / (end - cardprog_start) / 1024);
    }
}

void ps1_cardman_close(void) {
    if (fd < 0)
        return;
    ps1_cardman_flush();
    sd_close(fd);
    fd = -1;
}

void ps1_cardman_next_channel(void) {
    card_chan += 1;
    if (card_chan > CHAN_MAX)
        card_chan = CHAN_MIN;
}

void ps1_cardman_prev_channel(void) {
    card_chan -= 1;
    if (card_chan < CHAN_MIN)
        card_chan = CHAN_MAX;
}

void ps1_cardman_next_idx(void) {
    card_idx += 1;
    card_chan = CHAN_MIN;
}

void ps1_cardman_prev_idx(void) {
    card_idx -= 1;
    card_chan = CHAN_MIN;
    if ((card_idx == IDX_GAMEID) 
        && (card_game_id[0] == 0x00))
        card_idx = IDX_MIN;
    else if (card_idx < IDX_GAMEID)
        card_idx = IDX_GAMEID;
}

int ps1_cardman_get_idx(void) {
    return card_idx;
}

int ps1_cardman_get_channel(void) {
    return card_chan;
}

void ps1_cardman_set_gameid(const char* game_id) {
    memcpy(card_game_id, game_id, 0xFF);
    card_idx = IDX_GAMEID;
    card_chan = CHAN_MIN;
}

void ps1_cardman_get_gametext(char* const game_text) {
    memcpy(game_text, card_game_id, 0xFF);
}