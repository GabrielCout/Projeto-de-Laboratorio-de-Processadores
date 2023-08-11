#include <stdint.h>
#include "malloc.h"

int main(void) {
   uint32_t pages = 3;
   volatile void *p;
   
   p = (void *)malloc(pages);

   p=p;
}



