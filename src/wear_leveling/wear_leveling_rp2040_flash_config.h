// Copyright 2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#ifndef __ASSEMBLER__
#    include "hardware/flash.h"
#    include "flashmap.h"
#endif

// 2-byte writes
#ifndef BACKING_STORE_WRITE_SIZE
#    define BACKING_STORE_WRITE_SIZE 2
#endif

// 16k backing space allocated
#ifndef WEAR_LEVELING_BACKING_SIZE
#    define WEAR_LEVELING_BACKING_SIZE (16 * 1024)
#endif // WEAR_LEVELING_BACKING_SIZE

// 512b logical EEPROM
#ifndef WEAR_LEVELING_LOGICAL_SIZE
#    define WEAR_LEVELING_LOGICAL_SIZE (512)
#endif // WEAR_LEVELING_LOGICAL_SIZE

// Define how much flash space we have (defaults to lib/pico-sdk/src/boards/include/boards/***)
#ifndef WEAR_LEVELING_RP2040_FLASH_SIZE
#    define WEAR_LEVELING_RP2040_FLASH_SIZE (PICO_FLASH_SIZE_BYTES)
#endif

// Define the location of emulated EEPROM
#define WEAR_LEVELING_RP2040_FLASH_BASE FLASH_OFF_EEPROM
