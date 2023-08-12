#include "malloc.h"
#include "sizes.h"

free_area_t free_area[MAX_ORDER]; // 0 to MAX_ORDER
unsigned long bitmaps[first_word_in_bitmap_of_order(MAX_ORDER) + 1]; // All bitmaps together so it can be statically defined, as well as its size
uint32_t heap_start;
uint32_t heap_end;
uint8_t zero_page;

void initialize(uint32_t heap_init, uint32_t end_heap, uint8_t zero) {
    uint32_t size_in_bytes = end_heap - heap_init;
    uint32_t i = 0;
    struct list_addr *current;
    uint8_t order = 0;

    heap_start = heap_init;
    heap_end = end_heap;
    zero_page = zero;

    for (order = 0; order < MAX_ORDER; order++) {
        // Initialize the bitmap for this order
        uint32_t index = first_word_in_bitmap_of_order(order);
        if (index == MAX_ORDER-1) index++; // last one needs only half a long (32 bits), for the 64 MB heap
        free_area[order].map = &bitmaps[index]; // set the right addres for each one

        uint32_t map_size = bitmap_size_for_order(order);
        map_size = map_size == 0 ? 1 : map_size;
        for (i = 0; i < map_size; i++) {
            free_area[order].map[i] = 0;
        }

        current = &free_area[order].free_list;
        current->prev = current;
        current->next = current;
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

uint8_t empty_list(struct list_addr *list) {
    return list->next == list;
}

struct list_addr *pop_first_element(struct list_addr *list) {
    struct list_addr *return_addr;

    if (empty_list(list)) {
        return (struct list_addr *)0UL;
    }

    return_addr = list->next;
    if (return_addr->next == return_addr) { // last of the list, will be empty when removed
        list->next = list;
    }
    else {
        list->next = return_addr->next;
    }
    return_addr->next->prev = return_addr->prev;
    return return_addr;
}

void put_element(struct list_addr *list, struct list_addr *element) {
    if (empty_list(list)) {
        element->next = element;
        element->prev = list;
    }
    else {
        element->next = list->next;
        element->prev = list;
        element->next->prev = element;
    }
    list->next = element;
}

unsigned long get_block_from_order(size_t wanted_order, size_t found_block_order,unsigned long block_addr) {
    unsigned long return_addr;
    uint32_t i;

    for (i = 0; i < found_block_order-wanted_order; i++) {
        return_addr = block_addr + BLOCK_SIZE(found_block_order-i)/2;
        if (found_block_order-i != MAX_ORDER-1) { // invert the bit when my block is divided
            MARK_USED((block_addr-heap_start)/PAGE_SIZE, found_block_order-i, &free_area[found_block_order-i]);
        }
        put_element(&free_area[found_block_order-i-1].free_list, (struct list_addr *)block_addr);
        block_addr = return_addr;
    }

    return return_addr;
}

unsigned long allocate_pages(size_t order){
    size_t current_order;
    struct list_addr *current_list;
    unsigned long block_addr;

    for (current_order = order; current_order < MAX_ORDER; current_order++) {
        current_list = &free_area[current_order].free_list;
        if (!empty_list(current_list)) {
            block_addr = (unsigned long) pop_first_element(current_list); // found a block
            if (current_order != order) { // Not in the wanted order
                block_addr = get_block_from_order(order, current_order, block_addr); // divide it until the wanted order
            }
            MARK_USED((block_addr-heap_start)/PAGE_SIZE, order, &free_area[order]); // Invert bit for the actual allocation
            return block_addr;
        }
    }

    return -1; // No blocks found
}

unsigned long get_free_page() {
    unsigned long page_addr;
    uint32_t *i;

    page_addr = allocate_pages(0);

    if (zero_page == 1) {
        for (i = (uint32_t *) page_addr; ((unsigned long) i) < page_addr + PAGE_SIZE; i++) {
            (*i) &= 0UL;
        }
    }

    return page_addr;
}

unsigned long get_free_pages(size_t order) {
    unsigned long page_addr;

    page_addr = allocate_pages(order);

    return page_addr;
}

void *malloc(size_t size) {
    void *p = 0;
    size_t order;
    size_t block_size;
    struct block_order *block_header;

    if (size <= PAGE_SIZE) {
        order = 0;
        p = (void *) get_free_page();
    }
    else if (size > MAX_BLOCK_SIZE) { // can`t give one block
        return (void *)0UL;
    }
    else {
        order = 1;
        block_size = (PAGE_SIZE * (1UL << order));
        // calculate the minimum order necessary
        while (order < MAX_ORDER-1 && (size / block_size > 1 || (size / block_size == 1 && size % block_size > 0)))
        {
            order++;
            block_size = (PAGE_SIZE * (1UL << order));
        }
        p = (void *) get_free_pages(order);
    }

    block_header = p;
    block_header->order = order;
    return p + sizeof(struct block_order) + (sizeof(struct block_order) % 4);
}

unsigned long merge(unsigned long block_addr, size_t order) {
    unsigned long buddy_addr;

    buddy_addr = block_addr ^ (1 << order);
    pop_first_element(buddy_addr);

    return buddy_addr < block_addr ? buddy_addr : block_addr;
}

void free_pages_ok(void *block_addr, size_t order) {
    size_t current_order;
    unsigned long addr = block_addr;

    for (current_order = order;;current_order++) {
        if (current_order < MAX_ORDER - 1) {
            MARK_USED((addr-heap_start)/PAGE_SIZE, current_order, &free_area[current_order]);
        }
        if (BUDDY_IS_FREE((addr-heap_start)/PAGE_SIZE, current_order, &free_area[current_order]) && current_order < MAX_ORDER - 1) { // for the MAX_ORDER-1, it is always marked as free
            addr = merge(addr, current_order);
        }
        else {
            put_element(&free_area[current_order].free_list, (struct list_addr *)addr);
            break;
        }
    }
}

void free_page(void *block_addr) {
    free_pages_ok(block_addr, 0);
}

void free_pages(void *block_addr, size_t order) {
    free_pages_ok(block_addr, order);
}

void free(void *addr) {
    void *block_addr;
    size_t order;
    
    if (addr == 0) {
        return;
    }

    block_addr = addr - sizeof(struct block_order) - (sizeof(struct block_order) % 4);
    order = ((struct block_order *)block_addr)->order;

    if (order == 0) {
        free_page(block_addr);
    }
    else if (order < MAX_ORDER) {
        free_pages(block_addr, order);
    }
}

