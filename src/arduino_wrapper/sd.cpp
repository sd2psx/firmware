#include "config.h"

#include "SdFat.h"
#include "SPI.h"

#include "hardware/gpio.h"

extern "C" {
#include "psram.h"
#include "dirty.h"
}

#include <stdio.h>

SdFat sd;
File dataFile;

extern "C" void sd_init() {
    printf("SD start!\n");

    SPI1.setRX(SD_MISO);
    SPI1.setTX(SD_MOSI);
    SPI1.setSCK(SD_SCK);
    SPI1.setCS(SD_CS);

    sd.begin(SdSpiConfig(SD_CS, DEDICATED_SPI, SD_BAUD, &SPI1));

    printf("sd init done!!\n");
    static uint8_t buf[512];
    dataFile = sd.open("CARD.BIN", O_RDWR);
    if (dataFile) {
        printf("start read!\n");
        unsigned start = millis();
        int cnt = 0;
        while (dataFile.available()) {
            dataFile.read(buf, sizeof(buf));
            psram_write(cnt, buf, sizeof(buf));
            if (!cnt)
                printf("header %02X %02X %02X %02X\n", buf[0], buf[1], buf[2], buf[3]);
            cnt += sizeof(buf);
        }
        unsigned end = millis();
        if (cnt == 8 * 1024 * 1024) {
            float speed = (8 * 1024 * 1000.0) / (end - start);
            printf("done read! size OK! speed = %.2f\n", speed);
        }
    } else {
        printf("cannot open the file!\n");
    }
}

void sdCsInit(SdCsPin_t pin) {
    gpio_init(pin);
    gpio_set_dir(pin, 1);
}

void sdCsWrite(SdCsPin_t pin, bool level) {
    gpio_put(pin, level);
}

uint8_t flushbuf[512];

/* this goes through blocks in psram marked as dirty and flushes them to sd */
// TODO: this should be moved into C code and sd.cpp just contain wrapper funcs
extern "C" void dirty_task(void) {
    int num_after = 0;
    int hit = 0;
    uint64_t start = time_us_64();
    while (1) {
        if (!dirty_lockout_expired())
            break;
        /* do up to 100ms of work per call to dirty_taks */
        if ((time_us_64() - start) > 100 * 1000)
            break;

        dirty_lock();
        int sector = dirty_get_marked();
        num_after = num_dirty;
        if (sector == -1) {
            dirty_unlock();
            break;
        }
        psram_read(sector * 512, flushbuf, 512);
        dirty_unlock();

        ++hit;

        int seek = dataFile.seekSet(sector * 512);
        size_t wr = dataFile.write(flushbuf, 512);
        // TODO: check return code etc
    }
    dataFile.flush();

    uint64_t end = time_us_64();

    if (hit)
        printf("remain to flush - %d - this one flushed %d and took %d ms\n", num_after, hit, (int)((end - start) / 1000));
}
