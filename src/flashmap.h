#pragma once

/* application, including rp2040 bootloader */
#define FLASH_OFF_APP (0x0)

/* 4k before eeprom starts */
#define FLASH_OFF_CIV (0x7fb000)

/* 16k space before 8MB boundary */
#define FLASH_OFF_EEPROM (0x7fc000)

/* at the 8MB boundary */
#define FLASH_OFF_PS2EXP (0x800000)
