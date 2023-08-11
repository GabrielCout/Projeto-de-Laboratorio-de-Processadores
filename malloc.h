#include <stdint.h>
#include <stddef.h>

/*
 * Macros úteis
 */
#define bit_set(X, B) X |= (1 << B)
#define bit_clr(X, B) X &= ~(1 << B)
#define bit_inv(X, B) X ^= (1 << B)
#define bit_is_set(X, B) ( X & (1 << B) )

#ifndef HEAP_DEFAULT_SIZE
#define HEAP_DEFAULT_SIZE 67108864
#endif // 64 MB

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#ifndef MAX_PAGES
#define MAX_PAGES (HEAP_DEFAULT_SIZE/PAGE_SIZE)
#endif

#ifndef MAX_ORDER
#define MAX_ORDER 10
#endif

#define MAX_BLOCK_SIZE (PAGE_SIZE * (1 << (MAX_ORDER-1)))

struct list_addr {
	struct list_addr *prev, *next;
};

typedef struct free_area_struct {
    struct list_addr free_list;
    unsigned long    *map;
} free_area_t;

void initialize(uint32_t heap_init, uint32_t end_heap);

void *malloc(size_t size);

