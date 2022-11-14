#include "settings.h"

#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "wear_leveling/wear_leveling.h"

/* NOTE: for any change to the layout/size of this structure (that gets shipped to users),
   ensure to increase the version magic below -- this will trigger setting reset on next boot */
typedef struct {
    uint32_t version_magic;
    uint16_t ps1_card;
    uint16_t ps2_card;
    uint8_t ps1_channel;
    uint8_t ps2_channel;
    uint8_t ps1_flags; // TODO: single bit options: freepsxboot, pocketstation, freepsxboot slot
    // TODO: more ps1 settings: model for freepsxboot
    uint8_t ps2_flags; // TODO: single bit options: autoboot
    // TODO: display settings?
    // TODO: how do we store last used channel for cards that use autodetecting w/ gameid?
} settings_t;

#define SETTINGS_VERSION_MAGIC (0xABCD0001)

_Static_assert(sizeof(settings_t) == 12, "unexpected padding in the settings structure");

static settings_t settings;

static void settings_reset(void) {
    memset(&settings, 0, sizeof(settings));
    settings.version_magic = SETTINGS_VERSION_MAGIC;
    if (wear_leveling_write(0, &settings, sizeof(settings)) == WEAR_LEVELING_FAILED)
        fatal("failed to reset settings");
}

void settings_init(void) {
    printf("Settings - init\n");
    if (wear_leveling_init() == WEAR_LEVELING_FAILED) {
        printf("failed to init wear leveling, reset settings\n");
        settings_reset();

        if (wear_leveling_init() == WEAR_LEVELING_FAILED)
            fatal("cannot init eeprom emu");
    }

    wear_leveling_read(0, &settings, sizeof(settings));
    if (settings.version_magic != SETTINGS_VERSION_MAGIC) {
        printf("version magic mismatch, reset settings\n");
        settings_reset();
    }
    printf("read settings ps2_channel %d\n", settings.ps2_channel);
}

void settings_update(void) {
    int ret = wear_leveling_write(0, &settings, sizeof(settings));
    printf("write settings ret=%d\n", ret);
}

int settings_get_ps2_card(void) {
    return settings.ps2_card;
}

int settings_get_ps2_channel(void) {
    return settings.ps2_channel;
}

void settings_set_ps2_card(int x) {
    if (x != settings.ps2_card) {
        settings.ps2_card = x;
        // TODO: figure to update single field
        settings_update();
    }
}

void settings_set_ps2_channel(int x) {
    if (x != settings.ps2_channel) {
        // TODO: figure to update single field
        settings.ps2_channel = x;
        settings_update();
    }
}
