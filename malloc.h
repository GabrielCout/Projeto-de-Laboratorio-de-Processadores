#include <stdint.h>
#include <stddef.h>

/*
 * Macros úteis
 */
#define bit_set(X, B) X |= (1 << B)
#define bit_clr(X, B) X &= ~(1 << B)
#define bit_inv(X, B) X ^= (1 << B)
#define bit_is_set(X, B) ( X & (1 << B) )

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#ifndef MAX_ORDER
#define MAX_ORDER 10
#endif

#define MAX_BLOCK_SIZE PAGE_SIZE * (1 << (MAX_ORDER))

struct list_addr {
	struct list_addr *next, *prev;
    uint32_t addr;
};

typedef struct free_area_struct {
    struct list_addr free_list;
    unsigned long    *map;
} free_area_t;

void *malloc(size_t size);

