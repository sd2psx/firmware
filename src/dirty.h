#pragma once

#include <inttypes.h>
#include "hardware/sync.h"
#include "hardware/timer.h"
#include "pico/platform.h"

extern spin_lock_t *dirty_spin_lock;
extern volatile uint32_t dirty_lockout;

static inline uint64_t __time_critical_func(RAM_time_us_64)() {
    // Need to make sure that the upper 32 bits of the timer
    // don't change, so read that first
    uint32_t hi = timer_hw->timerawh;
    uint32_t lo;
    do {
        // Read the lower 32 bits
        lo = timer_hw->timerawl;
        // Now read the upper 32 bits again and
        // check that it hasn't incremented. If it has loop around
        // and read the lower 32 bits again to get an accurate value
        uint32_t next_hi = timer_hw->timerawh;
        if (hi == next_hi) break;
        hi = next_hi;
    } while (true);
    return ((uint64_t) hi << 32u) | lo;
}

static inline void __time_critical_func(dirty_lock)(void) {
    spin_lock_unsafe_blocking(dirty_spin_lock);
}

static inline void __time_critical_func(dirty_unlock)(void) {
    spin_unlock_unsafe(dirty_spin_lock);
}

static inline void __time_critical_func(dirty_lockout_renew)(void) {
    /* lockout for 100ms, store time in ms */
    dirty_lockout = (uint32_t)(RAM_time_us_64() / 1000) + 100;
}

static inline int __time_critical_func(dirty_lockout_expired)(void) {
    return dirty_lockout < (uint32_t)(RAM_time_us_64() / 1000);
}

void dirty_init(void);
int dirty_get_marked(void);
void dirty_mark(uint32_t sector);
void dirty_task(void);

extern int num_dirty;
