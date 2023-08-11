#include <stdint.h>
#include <stddef.h>
#include "sizes.h"

/*
 * Macros úteis
 */
#define bit_set(X, B) X |= (1UL << B)
#define bit_clr(X, B) X &= ~(1UL << B)
#define bit_inv(X, B) X ^= (1UL << B)
#define bit_is_set(X, B) ( X & (1 << B) )

#define MARK_USED(index, order, area)  change_bit((index) >> (1+(order)), (area)->map)
// index in mem_map array of a page is, given the initial heap addres and the page addres: (page_addr-heap_init)/PAGE_SIZE

struct list_addr {
	struct list_addr *prev, *next;
};

typedef struct free_area_struct {
    struct list_addr free_list;
    unsigned long    *map;
} free_area_t;

static inline void change_bit(int nr, volatile unsigned long *addr)
{
	unsigned long *p = ((unsigned long *)addr) + (nr) / BITS_PER_LONG;

	bit_inv((*p), (nr) % BITS_PER_LONG);
}

void initialize(uint32_t heap_init, uint32_t end_heap);

void *malloc(size_t size);

