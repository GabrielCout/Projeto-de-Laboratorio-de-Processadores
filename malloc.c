#include "malloc.h"

free_area_t free_area[MAX_ORDER];

void initialize(uint32_t heap_init, uint32_t end_heap) {
    uint32_t size_in_bytes = end_heap - heap_init;
    uint32_t i = 0;
    struct list_addr *current;
    struct list_addr *previous;
    uint8_t order = 0;

    for (order = 0; order < MAX_ORDER; order++) {
        free_area[order].map = NULL; // Alterar, tem q alocar o tamanho certo para cada um
    }

    for (i = 0, current = &free_area[MAX_ORDER-1].free_list; i < size_in_bytes; i += MAX_BLOCK_SIZE, current += 2*sizeof(struct list_addr *) + sizeof(uint32_t)) { // filling only the max order array (max block size)
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
