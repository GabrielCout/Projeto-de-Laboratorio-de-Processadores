#include "malloc.h"
#include "sizes.h"

free_area_t free_area[MAX_ORDER]; // array for each block from 0 to MAX_ORDER-1
unsigned long bitmaps[first_word_in_bitmap_of_order(MAX_ORDER) + 1]; // All bitmaps together so it can be statically defined, as well as its size
uint32_t heap_start;
uint32_t heap_end;
uint8_t zero_page;

/* Function to initialize each bitmap in free_area with the respective start addres for each
 * order bitmap in the global bitmap (that has all the bitmaps concatenated).
 * 
 * This function also initializes the heads for each order free_list, making it point to itself.
 * 
 * Also, the free_list of MAX_ORDER-1 is filled with all the blocks at that order. Their addresses are calculated and
 * put in the list. Each list node is located on the first two words of the respective free block, since this blocks
 * are free and, therefore, not being used by any program.
 */
void initialize(uint32_t heap_init, uint32_t end_heap, uint8_t zero) {
    uint32_t size_in_bytes = end_heap - heap_init; // heap size
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

    for (i = 0, current = &free_area[MAX_ORDER-1].free_list; i < size_in_bytes/MAX_BLOCK_SIZE; i++, current = current->next) { // filling only the max order array (max block size)
        if (i == 0) {
            current->prev = current; // If current->prev == current, its the head of the list
        }
        else if (i == 1) {
            current->prev = &free_area[MAX_ORDER-1].free_list; // head
        }
        else {
            current->prev = (struct list_addr *) (heap_init + (i-2) * MAX_BLOCK_SIZE);
        }
        current->next = (struct list_addr *) (heap_init + i * MAX_BLOCK_SIZE);

        if (i == size_in_bytes/MAX_BLOCK_SIZE-1) { // last node
            current = current->next;
            current->prev = (struct list_addr *) (heap_init + (i-1) * MAX_BLOCK_SIZE);
            current->next = current; // End of list, points to itself
        }
    }
}

/*
 *  Checks if the elemetn is the head (first in the list)
 */
uint8_t first_element(struct list_addr *element) {
    return element->prev == element;
}

/*
 *  Checks if the elemetn is the tail (last in the list)
 */
uint8_t last_element(struct list_addr *element) {
    return element->next == element;
}

/*
 *  Checks if the list is empty (head is the first and last element)
 */
uint8_t empty_list(struct list_addr *list) {
    return first_element(list) && last_element(list);
}

/*
 *  Removes from the list the element pointed by the head as the next and returns it
 */
struct list_addr *pop_first_element(struct list_addr *list) {
    struct list_addr *return_addr;

    if (empty_list(list)) {
        return (struct list_addr *)0UL;
    }

    return_addr = list->next;
    if (last_element(return_addr)) { // last of the list, that will be empty when removed
        list->next = list;
    }
    else {
        list->next = return_addr->next;
    }
    return_addr->next->prev = return_addr->prev;
    return return_addr;
}

/*
 *  Puts an element on the list right after the head
 */
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

/*
 *  Removes from the list the given element
 */
void remove_from_list(struct list_addr *list, struct list_addr *element) {
    struct list_addr *current_element;
    
    if (empty_list(list)) {
        return;
    }

    for (current_element = list; !last_element(current_element); current_element = current_element->next) {
        if(current_element->next == element) {
            pop_first_element(current_element);
        }
    }
}






/*
 *  Receives a block addres and its respective order, so the block is in the free list of this order; receives
 *  as well the wanted order for the block to be allocated.
 *  
 *  Removes the block from its order free_list, halves it into two buddies, puts one of the buddies on the next
 *  smaller order free_list and checks if the other buddy is from the wanted order. If isn't, halves this last block
 *  and repeats this process until a block from the wanted order is got.
 * 
 *  Each time a block is removed from a free_list and halved, its buddy bit is inverted. This isn't done if the block
 *  is from the MAX_ORDER-1, because, as it is the order with the starting list, there are no buddies in this order.
 *  
 *  Returns the address for the block of the wanted order.
 */
unsigned long get_block_from_order(size_t wanted_order, size_t found_block_order,unsigned long block_addr) {
    unsigned long return_addr;
    uint32_t i;

    for (i = 0; i < found_block_order-wanted_order; i++) {
        return_addr = block_addr + BLOCK_SIZE(found_block_order-i)/2; // calculates the buddy for this block in the next low order
        if (found_block_order-i != MAX_ORDER-1) { // invert the bit when my block is divided
            MARK_USED((block_addr-heap_start)/PAGE_SIZE, found_block_order-i, &free_area[found_block_order-i]);
        }
        put_element(&free_area[found_block_order-i-1].free_list, (struct list_addr *)block_addr); // puts the block in the next low order free_list
        block_addr = return_addr;
    }

    return return_addr;
}


/*
 *  Returns the address for the block of the wanted order, allocating it.
 *  
 *  Checks the free_lists from the wanted order to the MAX_ORDER-1 for free blocks.
 *  If the found block isn't in the wanted order list, the get_block_from_order function
 *  is called so the block is divide until it is a blcok from the wanted order.
 */
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

/*
 *  Returns the address for a order 0 block, that corresponds to one page.
 *  
 *  Clean the page if the allocator is configured for doing it.
 * 
 *  Calls the main allocator function.
 */
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

/*
 *  Returns the address for a block of the wanted order, that corresponds to 2^(order) pages.
 * 
 *  Calls the main allocator function.
 */
unsigned long get_free_pages(size_t order) {
    unsigned long page_addr;

    page_addr = allocate_pages(order);

    return page_addr;
}

/*
 *  Allocation function exposed to be called by other programs.
 * 
 *  It calculates the necessary block order to allocate for a received size in bytes.
 * 
 *  Returns the block address as a pointer. The addres returned is actually the addres after the header
 *  of the block, that will store the order of the block so it can be correctly freed.
 */
void *malloc(size_t size) {
    void *p = 0;
    size_t order;
    size_t block_size;
    struct block_order *block_header;

    if (size <= PAGE_SIZE) {
        order = 0;
        p = (void *) get_free_page();
    }
    else if (size > MAX_BLOCK_SIZE) { // can't give one MAX_ORDER-1 block
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






/*
 *  Merges a block with its buddy in the given order and returns the new block addres.
 *  
 *  The buddy is removed from the order free_list and the smaller addres between the bock and its buddy is returned
 *  as the new block addres, that is and order+1 block.
 */
unsigned long merge(unsigned long block_addr, size_t order) {
    unsigned long buddy_addr;

    buddy_addr = ((block_addr-heap_start) ^ (1 << (order+PAGE_SHIFT))) + heap_start;
    remove_from_list(&free_area[order].free_list, (struct list_addr *)buddy_addr);

    return buddy_addr < block_addr ? buddy_addr : block_addr;
}

/*
 *  Frees the block from the given order and addres.
 *  
 *  When the block is freed, its order buddy bit is inverted. If, after the inversion, the bit value is 0,
 *  then the block and its buddy have the same status. As the block is now free, its buddy was already free
 *  and in their order free_list. If so, they are merged into a order+1 block.
 * 
 *  The same process is repeated for the new created block and is repeated until a new block can't be merged with
 *  its buddy. This situation can happen if the buddy of the block isn't free (after the inversion, their buddy
 *  bit is 1) or the new block is an MAX_ORDER-1 block (doesn't have a buddy). In both cases, the block is put
 *  into its order free_list.
 */
void free_pages_ok(void *block_addr, size_t order) {
    size_t current_order;
    unsigned long addr = (unsigned long) block_addr;

    for (current_order = order;;current_order++) {
        if (current_order < MAX_ORDER - 1) {
            MARK_USED((addr-heap_start)/PAGE_SIZE, current_order, &free_area[current_order]); // when freed, the bit for the block and its buddy is inverted
        }
        if (BUDDY_IS_FREE((addr-heap_start)/PAGE_SIZE, current_order, &free_area[current_order]) && current_order < MAX_ORDER - 1) { // for the MAX_ORDER-1, it is always marked as free
            addr = merge(addr, current_order); // merges the block with its buddy, as it is also free
        }
        else {
            put_element(&free_area[current_order].free_list, (struct list_addr *)addr); // puts the block in free_list
            break;
        }
    }
}


/*
 *  Frees the parameter order 0 block (one page)
 */
void free_page(void *block_addr) {
    free_pages_ok(block_addr, 0);
}

/*
 *  Frees the parameter block of the given order
 */
void free_pages(void *block_addr, size_t order) {
    free_pages_ok(block_addr, order);
}

/*
 *  Frees the block in the given addres.
 *  
 *  The block order is recovered from its header and the actual block adddres is used
 *  to free it.
 */
void free(void *addr) {
    void *block_addr;
    size_t order;
    
    if ((unsigned long) addr < heap_start) {
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

