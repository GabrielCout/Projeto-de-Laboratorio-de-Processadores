#include <stdint.h>
#include "malloc.h"

int main(void) {
   uint32_t pages = 3;
   void *p, *q, *r, *s, *t, *a, *b, *c;
   
   a = (void *)malloc(2048*sizeof(int));
   b = (void *)malloc(4096*sizeof(int));
   c = (void *)malloc(4097*sizeof(int));
   p = (void *)malloc(pages);
   q = (void *)malloc(pages*sizeof(int));
   free(p);
   r = (void *)malloc(pages*sizeof(int));
   free(c);
   s = (void *)malloc(1030*sizeof(int));
   free(a);
   free(q);
   free(r);
   free(b);
   t = (void *)malloc(1024*sizeof(int));
   free(s);
   free(t);


   // p=p;
   // a = (void *)(a - b);
   // c = (void *)(c - p);
   // q = (void *)(q - r);
   // s = (void *)(s - t);
}



