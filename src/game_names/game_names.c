
#include "game_names.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../debug.h"
#include "../sd.h"
#include "../settings.h"

#define MAX_GAME_NAME_LENGTH (127)
#define MAX_PREFIX_LENGTH    (4)
#define MAX_GAME_ID_LENGTH   (16)

extern const char _binary_gamedbps1_dat_start, _binary_gamedbps1_dat_size;
// extern const char _binary_gamedbps2_dat_start, _binary_gamedbps2_dat_size;

static int card_idx;
static int card_chan;
static char card_game_id[MAX_GAME_ID_LENGTH];

static bool game_names_sanity_check_game_id(const char* const game_id) {
    uint8_t i = 0U;

    char splittable_game_id[MAX_GAME_ID_LENGTH];
    strlcpy(splittable_game_id, game_id, MAX_GAME_ID_LENGTH);
    char* prefix = strtok(splittable_game_id, "-");
    char* id = strtok(NULL, "-");

    while (prefix[i] != 0x00) {
        if (!isalpha((int)prefix[i])) {
            return false;
        }
        i++;
    }
    if (i == 0) {
        return false;
    } else {
        i = 0;
    }

    while (prefix[i] != 0x00) {
        if (!isdigit((int)id[i])) {
            return false;
        }
        i++;
    }

    return (i > 0);
}

static uint32_t game_names_char_array_to_uint32(const char in[4]) {
#if BIG_ENDIAN
    char inter[4] = {in[3], in[2], in[1], in[0]};
#else
    char* inter = in;
#endif
    return *(uint32_t*)inter;
}

static uint32_t game_names_find_prefix_offset(uint32_t numericPrefix, const char* const db_start) {
    uint32_t offset = 0;

    const char* pointer = db_start;

    while (offset == 0) {
        uint32_t currentprefix = game_names_char_array_to_uint32(pointer), currentoffset = game_names_char_array_to_uint32(&pointer[4]);

        if (currentprefix == numericPrefix) {
            offset = currentoffset;
        }
        if ((currentprefix == 0U) && (currentoffset == 0U)) {
            break;
        }
        pointer += 8;
    }

    return offset;
}

const char* game_names_get_game_name(const char* const id) {
    char prefixString[MAX_PREFIX_LENGTH + 1] = {};
    char idString[10] = {};
    const char* card_game_name = NULL;

    //    const char* const db_start = settings_get_mode() == MODE_PS1 ? &_binary_gamedbps1_dat_start : &_binary_gamedbps2_dat_start;
    //    const char* const db_size = settings_get_mode() == MODE_PS1 ? &_binary_gamedbps1_dat_size : &_binary_gamedbps2_dat_size;

    const char* const db_start = &_binary_gamedbps1_dat_start;
    const char* const db_size = &_binary_gamedbps1_dat_size;

    uint32_t numericPrefix = 0, prefixOffset = 0, currentId = 0, numericId = 0;

    if (game_names_sanity_check_game_id(id)) {
        char* copy = strdup(id);
        char* split = strtok(copy, "-");

        if (strlen(split) > 0) {
            strlcpy(prefixString, split, MAX_PREFIX_LENGTH + 1);
            for (uint8_t i = 0; i < MAX_PREFIX_LENGTH; i++) {
                prefixString[i] = toupper((unsigned char)prefixString[i]);
            }
            numericPrefix = game_names_char_array_to_uint32(prefixString);
        }

        split = strtok(NULL, "-");

        if (strlen(split) > 0) {
            strlcpy(idString, split, 11);
            numericId = atoi(idString);
        }

        prefixOffset = game_names_find_prefix_offset(numericPrefix, db_start);

        if (prefixOffset < (size_t)db_size) {
            uint32_t offset = prefixOffset;
            do {
                currentId = game_names_char_array_to_uint32(&(db_start)[offset]);
                if (currentId == numericId) {
                    uint32_t name_offset = game_names_char_array_to_uint32(&(db_start)[offset + 4]);

                    debug_printf("Found ID - Name Offset: %d, Parent ID: %d\n", (int)name_offset,
                                 (int)game_names_char_array_to_uint32(&(db_start)[offset + 8]));

                    snprintf(card_game_id, MAX_GAME_ID_LENGTH, "%s-%0*d", prefixString, (int)strlen(idString),
                             (int)game_names_char_array_to_uint32(&(db_start)[offset + 8]));

                    debug_printf("Parent ID: %s\n", card_game_id);

                    if ((name_offset < (size_t)db_size) && (db_start + name_offset) != 0x00) {
                        card_game_name = (db_start + name_offset);
                        debug_printf("Name:%s\n", card_game_name);

                        return card_game_name;
                    } else {
                        return NULL;
                    }
                }
                offset += 12;
            } while (currentId != 0);
        }
    }

    return NULL;
}

const char* game_names_name_from_file(char* path, size_t size, int channel) {
    int dir, it;
    char filename[MAX_GAME_NAME_LENGTH] = {};

    dir = sd_open(path, O_RDONLY);
    if (dir >= 0) {
        it = sd_iterate_dir(dir, it);
        while (it != -1) {
            int i = MAX_GAME_NAME_LENGTH - 1;
            sd_get_name(it, filename, MAX_GAME_NAME_LENGTH);
            while ((filename[i] == 0x0) && (i > 0))
                i--;
            if ((i > 4) && (filename[i--] == 't') && (filename[i--] == 'x') && (filename[i--] == 't')) {

            }
        }
    }
}