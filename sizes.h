#include <stdint.h>
#include <stddef.h>

#ifndef WORD_SIZE
#define WORD_SIZE (8*sizeof(unsigned long))
#endif // 32 B

#ifndef BITS_PER_LONG
#define BITS_PER_LONG WORD_SIZE
#endif

#ifndef HEAP_SIZE
#define HEAP_SIZE 67108864
#endif // 64 MB

#ifndef PAGE_SHIFT
#define PAGE_SHIFT 12
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE (1UL << PAGE_SHIFT)
#endif // 4096 B

#ifndef MAX_PAGES
#define MAX_PAGES (HEAP_SIZE/PAGE_SIZE)
#endif

#ifndef MAX_ORDER
#define MAX_ORDER 10
#endif

#define BLOCK_SIZE(order) (PAGE_SIZE * (1UL << order))
#define MAX_BLOCK_SIZE BLOCK_SIZE(MAX_ORDER-1)