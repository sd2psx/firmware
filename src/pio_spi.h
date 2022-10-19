/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _PIO_SPI_H
#define _PIO_SPI_H

#include "hardware/pio.h"
#include "spi.pio.h"

#define PIO_SPI_DMA_RX_CHAN 0
#define PIO_SPI_DMA_TX_CHAN 1

typedef struct pio_spi_inst {
    PIO pio;
    uint sm;
    uint cs_pin;
} pio_spi_inst_t;

void pio_spi_write8_blocking(const pio_spi_inst_t *spi, const uint8_t *src, size_t len);

void pio_spi_read8_blocking(const pio_spi_inst_t *spi, uint8_t *dst, size_t len);

void pio_spi_write8_read8_blocking(const pio_spi_inst_t *spi, uint8_t *src, size_t srclen, uint8_t *dst, size_t dstlen);

void pio_qspi_write8_read8_blocking(const pio_spi_inst_t *spi, uint8_t *src, size_t srclen, uint8_t *dst, size_t dstlen);

void pio_qspi_write8_read8_dma(const pio_spi_inst_t *spi, uint8_t *src, size_t srclen, uint8_t *dst, size_t dstlen);

void pio_qspi_dma_init(const pio_spi_inst_t *spi);

#endif
