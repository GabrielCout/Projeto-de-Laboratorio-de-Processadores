#include <stdint.h>
#include "malloc.h"

int main(void) {
   uint32_t pages = 3;
   void *p, *q, *r, *s, *t, *a, *b, *c;
   
   a = malloc(2048*sizeof(int)); // 8192 bytes -> 2 pages -> order 1
   b = malloc(4096*sizeof(int)); // 16384 bytes -> 4 pages -> order 2
   c = malloc(4097*sizeof(int)); // 16388 bytes -> at least 5 pages -> order 3 (8 pages)
   p = malloc(pages); // 3 bytes -> 1 page -> order 0
   q = malloc(pages*sizeof(int)); // 12 bytes -> 1 page -> order 0
   free(p);
   r = malloc(pages*sizeof(int)); // 12 bytes -> 1 page -> order 0
   free(c);
   s = malloc(1030*sizeof(int)); // 4120 bytes -> 2 pages -> order 1
   free(a);
   free(q);
   free(r);
   free(b);
   t = malloc(1024*sizeof(int)); // 4096 -> 1 page -> order 0
   free(s);
   free(t);
}



