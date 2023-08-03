#include "malloc.h"

free_area_t free_area[MAX_ORDER];

void initialize(uint32_t heap_init, uint32_t stack_init) {
    uint32_t size_in_bytes = stack_init - heap_init;
    uint32_t i = 0;
    struct list_addr *current;
    struct list_addr *previous;
    uint8_t j = 0;

    for (j = 0; j < MAX_ORDER; j++) {
        free_area[j].map = NULL;
    }

    for (i = 0, current = &free_area[MAX_ORDER]; i < size_in_bytes; i += MAX_BLOCK_SIZE, current += 2*sizeof(struct list_addr *) + sizeof(uint32_t)) {
        if (i == 0) {
            current->prev = NULL;
        }
        else {
            current->prev = previous;
            previous->next = current;
        }
        current->next = NULL;
        current->addr = heap_init + i;
        previous = current;
    }
}

void *malloc(size_t size) {
    void *p = 0;
    return p;
}
