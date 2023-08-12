#include <stdint.h>
#include "malloc.h"

int main(void) {
   uint32_t pages = 3;
   volatile void *p, *q, *r, *s, *t;
   
   p = (void *)malloc(pages);
   q = (void *)malloc(pages*sizeof(int));
   r = (void *)malloc(pages*sizeof(int));
   s = (void *)malloc(1030*sizeof(int));
   t = (void *)malloc(1024*sizeof(int));

   p=p;
}



