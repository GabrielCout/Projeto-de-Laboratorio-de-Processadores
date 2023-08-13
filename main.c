#include <stdint.h>
#include "malloc.h"

int main(void) {
   uint32_t pages = 3;
   void *p, *q, *r, *s, *t, *a, *b, *c;
   
   a = malloc(2048*sizeof(int));
   b = malloc(4096*sizeof(int));
   c = malloc(4097*sizeof(int));
   p = malloc(pages);
   q = malloc(pages*sizeof(int));
   free(p);
   r = malloc(pages*sizeof(int));
   free(c);
   s = malloc(1030*sizeof(int));
   free(a);
   free(q);
   free(r);
   free(b);
   t = malloc(1024*sizeof(int));
   free(s);
   free(t);


   // p=p;
   // a = (void *)(a - b);
   // c = (void *)(c - p);
   // q = (void *)(q - r);
   // s = (void *)(s - t);
}



