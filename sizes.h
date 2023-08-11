#include <stdint.h>
#include <stddef.h>

#ifndef WORD_SIZE
#define WORD_SIZE (8*sizeof(unsigned long))
#endif // 64 MB

#ifndef BITS_PER_LONG
#define BITS_PER_LONG WORD_SIZE
#endif

#ifndef HEAP_SIZE
#define HEAP_SIZE 67108864
#endif // 64 MB

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#ifndef MAX_PAGES
#define MAX_PAGES (HEAP_SIZE/PAGE_SIZE)
#endif

#ifndef MAX_ORDER
#define MAX_ORDER 10
#endif

#define MAX_BLOCK_SIZE (PAGE_SIZE * (1 << (MAX_ORDER-1)))