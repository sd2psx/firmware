#include "dirty.h"
#include "psram.h"
#include "cardman.h"

#include <stdio.h>

spin_lock_t *dirty_spin_lock;
volatile uint32_t dirty_lockout;

int num_dirty, dirty_activity;
static uint16_t dirty_heap[8 * 1024 * 1024 / 512];
static uint8_t dirty_map[8 * 1024 * 1024 / 512]; // TODO: make an actual bitmap to save 8x mem?

#define SWAP(a, b) do { \
    uint16_t tmp = a; \
    a = b; \
    b = tmp; \
} while (0);

void dirty_init(void) {
    dirty_spin_lock = spin_lock_init(spin_lock_claim_unused(1));
}

void __time_critical_func(dirty_mark)(uint32_t sector) {
    if (sector < sizeof(dirty_map)) {
        /* already marked? */
        if (dirty_map[sector])
            return;

        /* update map */
        dirty_map[sector] = 1;

        /* update heap */
        int cur = num_dirty++;
        dirty_heap[cur] = sector;
        while (dirty_heap[cur] < dirty_heap[(cur-1)/2]) {
            SWAP(dirty_heap[cur], dirty_heap[(cur-1)/2]);
            cur = (cur-1)/2;
        }
    }
}

static void heapify(int i) {
    int l = i * 2 + 1;
    int r = i * 2 + 2;
    int best = i;
    if (l < num_dirty && dirty_heap[l] < dirty_heap[best])
        best = l;
    if (r < num_dirty && dirty_heap[r] < dirty_heap[best])
        best = r;
    if (best != i) {
        SWAP(dirty_heap[i], dirty_heap[best]);
        heapify(best);
    }
}

int dirty_get_marked(void) {
    if (num_dirty == 0)
        return -1;

    uint16_t ret = dirty_heap[0];

    /* update heap */
    dirty_heap[0] = dirty_heap[--num_dirty];
    heapify(0);

    /* update map */
    dirty_map[ret] = 0;

    return ret;
}

/* this goes through blocks in psram marked as dirty and flushes them to sd */
void dirty_task(void) {
    static uint8_t flushbuf[512];

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

        if (cardman_write_sector(sector, flushbuf) != 0) {
            // TODO: do something if we get too many errors?
            // for now lets push it back into the heap and try again later
            printf("!! writing sector 0x%x failed\n", sector);

            dirty_lock();
            dirty_mark(sector);
            dirty_unlock();
        }
    }
    /* to make sure writes hit the storage medium */
    cardman_flush();

    uint64_t end = time_us_64();

    if (hit)
        printf("remain to flush - %d - this one flushed %d and took %d ms\n", num_after, hit, (int)((end - start) / 1000));

    if (num_after || !dirty_lockout_expired())
        dirty_activity = 1;
    else
        dirty_activity = 0;
}
