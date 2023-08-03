#include "malloc.h"

free_area_t free_area[MAX_ORDER];
uint32_t MAX_BLOCK_SIZE = PAGE_SIZE + (1 << (MAX_ORDER));

void initialize(uint32_t heap_init, uint32_t stack_init) {
    uint32_t size_in_bytes = stack_init - heap_init;
    uint32_t i = 0;
    struct list_addr *current[MAX_ORDER];
    uint8_t j = 0;

    for (j = 0; j < MAX_ORDER; j++) {
        current[j] = &free_area[j];
        free_area[j].map = NULL;
    }



    for (i = 0; i < size_in_bytes; i += PAGE_SIZE) {
        if (i == 0) {
            current[MAX_ORDER]->prev = NULL;
            current[MAX_ORDER]->next = NULL;
            current[MAX_ORDER]->addr = heap_init + i;
        }
        else {
            current[MAX_ORDER]->prev = 
        }
    }
}

void *malloc(size_t size) {
    void *p = 0;
    return p;
}
