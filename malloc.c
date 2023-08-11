#include "malloc.h"
#include "sizes.h"

free_area_t free_area[MAX_ORDER]; // 0 to MAX_ORDER
unsigned long bitmaps[(MAX_PAGES-MAX_PAGES/(1UL << (MAX_ORDER)))/BITS_PER_LONG + 1]; // All bitmaps together so it can be statically defined, as well as its size
uint32_t heap_start;
uint32_t heap_end;

void initialize(uint32_t heap_init, uint32_t end_heap) {
    uint32_t size_in_bytes = end_heap - heap_init;
    uint32_t i = 0;
    struct list_addr *current;
    uint8_t order = 0;

    heap_start = heap_init;
    heap_end = end_heap;

    for (order = 0; order < MAX_ORDER; order++) {
        // Initialize the bitmap for this order
        uint32_t index = (MAX_PAGES-MAX_PAGES/(1UL << (order)))/BITS_PER_LONG;
        if (index == MAX_ORDER-1) index++; // last one needs only half a long (32 bits), for the 64 MB heap
        free_area[order].map = &bitmaps[index]; // set the right addres for each one

        uint32_t map_size = (MAX_PAGES/(1UL << (order+1)))/BITS_PER_LONG;
        map_size = map_size == 0 ? 1 : map_size;
        for (i = 0; i < map_size; i++) {
            free_area[order].map[i] = 0;
        }
    }

    for (i = 0, current = &free_area[MAX_ORDER-1].free_list; i<size_in_bytes/MAX_BLOCK_SIZE; i++, current = current->next) { // filling only the max order array (max block size)
        if (i == 0) { // problema na condição
            current->prev = current; // If current->prev == current, its the head of the list
        }
        else if (i == 1) {
            current->prev = &free_area[MAX_ORDER-1].free_list; // head
        }
        else {
            current->prev = (struct list_addr *) (heap_init + (i-2) * MAX_BLOCK_SIZE);
        }
        current->next = (struct list_addr *) (heap_init + i * MAX_BLOCK_SIZE);

        if (i == size_in_bytes/MAX_BLOCK_SIZE-1) {
            current = current->next;
            current->prev = (struct list_addr *) (heap_init + (i-1) * MAX_BLOCK_SIZE);
            current->next = current; // End of list, points to itself
        }
    }
}

void *malloc(size_t size) {
    void *p = 0;
    return p;
}
