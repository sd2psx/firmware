#pragma once

#include <inttypes.h>
#include "hardware/sync.h"
#include "hardware/timer.h"

extern spin_lock_t *dirty_spin_lock;
extern volatile uint32_t dirty_lockout;

static inline void dirty_lock(void) {
    spin_lock_unsafe_blocking(dirty_spin_lock);
}

static inline void dirty_unlock(void) {
    spin_unlock_unsafe(dirty_spin_lock);
}

static inline void dirty_lockout_renew(void) {
    /* lockout for 100ms, store time in ms */
    dirty_lockout = (uint32_t)(time_us_64() / 1000) + 100;
}

static inline int dirty_lockout_expired(void) {
    return dirty_lockout < (uint32_t)(time_us_64() / 1000);
}

void dirty_init(void);
int dirty_get_marked(void);
void dirty_mark(uint32_t sector);
void dirty_task(void);

extern int num_dirty;
