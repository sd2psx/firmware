#include "dirty.h"

spin_lock_t *dirty_spin_lock;
volatile uint32_t dirty_lockout;

int num_dirty;
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
