/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pio_spi.h"
#include "config.h"

void __time_critical_func(pio_spi_write8_read8_blocking)(const pio_spi_inst_t *spi, uint8_t *src, size_t srclen, uint8_t *dst,
                                                         size_t dstlen) {
    io_rw_8 *txfifo = (io_rw_8 *) &spi->pio->txf[spi->sm];
    io_rw_8 *rxfifo = (io_rw_8 *) &spi->pio->rxf[spi->sm];

    while (srclen) {
        if (!pio_sm_is_tx_fifo_full(spi->pio, spi->sm)) {
            *txfifo = *src++;
            (void) *rxfifo;
            --srclen;
        }
    }

    while (dstlen) {
        if (!pio_sm_is_rx_fifo_empty(spi->pio, spi->sm)) {
            *txfifo = 0;
            *dst++ = *rxfifo;
            --dstlen;
        }
    }
}

void __time_critical_func(pio_qspi_write8_read8_blocking)(const pio_spi_inst_t *spi, uint8_t *src, size_t srclen, uint8_t *dst,
                                                          size_t dstlen) {
    io_rw_8 *txfifo = (io_rw_8 *) &spi->pio->txf[spi->sm];
    io_rw_8 *rxfifo = (io_rw_8 *) &spi->pio->rxf[spi->sm];

    // TODO: this should be done nicer. while it's safe (since we drive the clock), it can be done much faster
    pio_sm_set_pindirs_with_mask(spi->pio, spi->sm,
        (1 << (PSRAM_DAT+0)) | (1 << (PSRAM_DAT+1)) | (1 << (PSRAM_DAT+2)) | (1 << (PSRAM_DAT+3)),
        (1 << (PSRAM_DAT+0)) | (1 << (PSRAM_DAT+1)) | (1 << (PSRAM_DAT+2)) | (1 << (PSRAM_DAT+3)));
    while (srclen) {
        if (!pio_sm_is_tx_fifo_full(spi->pio, spi->sm)) {
            *txfifo = *src++;
            (void) *rxfifo;
            --srclen;
        }
    }
    // TODO: this should be done nicer. while it's safe (since we drive the clock), it can be done much faster
    pio_sm_set_pindirs_with_mask(spi->pio, spi->sm,
        0,
        (1 << (PSRAM_DAT+0)) | (1 << (PSRAM_DAT+1)) | (1 << (PSRAM_DAT+2)) | (1 << (PSRAM_DAT+3)));

    while (dstlen) {
        if (!pio_sm_is_rx_fifo_empty(spi->pio, spi->sm)) {
            *txfifo = 0;
            *dst++ = *rxfifo;
            --dstlen;
        }
    }
}
