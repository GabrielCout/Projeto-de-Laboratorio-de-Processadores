Da [explicação](https://www.kernel.org/doc/gorman/html/understand/understand009.html):

# Seção 6.0 - Physical Page Allocation

- Memory is broken up into large blocks of pages
- Each block is a power of two number of pages
- If a block of the desired size is not available, a large block is broken up in half
- The two blocks are buddies to each other
- One half is used for the allocation
- The other half is free
- The blocks are continuously halved as necessary until a block of the desired size is available
- When a block is later freed, the buddy is examined and the two coalesced if it is free

# 6.1 - Managing Free Blocks

- The allocator maintains blocks of free pages
- Each block is a power of two number of pages
- The exponent for the power of two sized block is referred to as the _order_
- An array of **free_area_t** structs are maintained for each order that points to a linked list of blocks of pages that are free
![Free page block management](https://www.kernel.org/doc/gorman/html/understand/understand-html029.png)

- This eliminates the chance that a larger block will be split to satisfy a request where a smaller block would have sufficed

- Each zone has a free_area_t struct array called free_area[MAX_ORDER]
- It is declared in <linux/mm.h>
```
typedef struct free_area_struct {
          struct list_head        free_list;
          unsigned long           *map;
 } free_area_t;
```
- Then
    - **free_list** A linked list of free page blocks;
    - **map** A bitmap with each bit representing the state of a pair of buddies.

    - Actually in [here](https://elixir.bootlin.com/linux/latest/source/include/linux/mmzone.h)
    - list_head is defined as [so](https://elixir.bootlin.com/linux/latest/source/include/linux/types.h)

- Linux uses one bit instead of two to represent each pair of buddies
    - Each time a buddy is allocated or freed, the bit representing the pair of buddies is toggled
    - The bit is zero if the pair of pages are both free or both full
    - 1 if only one buddy is in use
    - To toggle the correct bit, the macro MARK_USED() in page_alloc.c is used
```
#define MARK_USED(index, order, area) \
         __change_bit((index) >> (1+(order)), (area)->map)
```
- Then
    - **index** is the index of the page within the global **mem_map** array
    - By shifting it right by 1+order bits, the bit within **map** representing the pair of buddies is revealed. The aditional shift is because it is addressing pairs of consecutive blocks (and the number of pages on a block is always power of 2, so the order 0 pairs are always X0 and X1), not blocks

    - __change_bit can be [this](https://elixir.bootlin.com/linux/v4.19.154/source/include/asm-generic/bitops/non-atomic.h#L41) or [that](https://github.com/torvalds/linux/blob/master/arch/alpha/include/asm/bitops.h#L122)

# 6.2 - Allocating Pages

- The API for the allocation of page frames take a **gfp_mask** as a parameter
- **gfp_mask** is a set of flags that determine how the allocator will behave
- The allocation API functions all use the core function **__alloc_pages()**
- The APIs exist so that the correct node and zone will be chosen.
- API
    - `struct page * alloc_page(unsigned int gfp_mask)` - Allocate a single page and return a struct address
    - `struct page * alloc_pages(unsigned int gfp_mask, unsigned int order)` - Allocate 2<sup>*order*</sup> number of pages and returns a struct page
    - `unsigned long get_free_page(unsigned int gfp_mask)` - Allocate a single page, zero it and return a virtual address
    - `unsigned long __get_free_page(unsigned int gfp_mask)` - Allocate a single page and return a virtual address
    - `unsigned long __get_free_pages(unsigned int gfp_mask, unsigned int order)` - Allocate 2<sup>*order*</sup> number of pages and return a virtual address
- If a free block cannot be found of the requested order, a higher order block is split into two buddies
- One is allocated and the other is placed on the free list for the lower order
- Example
![Allocating physical pages](https://www.kernel.org/doc/gorman/html/understand/understand-html030.png)
- Then
    - When the block is later freed, the buddy will be checked
    - If both are free, they are merged to form a higher order block and placed on the higher free list where its buddy is checked and so on
    - If the buddy is not free, the freed block is added to the free list at the current order

- The second decision to make is which memory node or [**pg_data_t**](https://elixir.bootlin.com/linux/latest/source/include/linux/mmzone.h#L1374) to use
- Function **__alloc_pages()** for a UMA (see [**mm/page_alloc.c**](https://github.com/torvalds/linux/blob/master/mm/page_alloc.c#L4441))
    - This function, which is never called directly, examines the selected zone and checks if it is suitable to allocate from based on the number of available pages
    - If number of free pages reaches the **pages_low** watermark, it will wake **kswapd** to begin freeing up pages from zones and if memory is extremely tight, the caller will do the work of **kswapd** itself
    ![Call Graph: **alloc_pages()**](https://www.kernel.org/doc/gorman/html/understand/understand-html031.png)
    - The function [**rmqueue()**](https://github.com/torvalds/linux/blob/master/mm/page_alloc.c#L2807) is called to allocate the block of pages or split higher level blocks if one of the appropriate size is not available

- Commadn to debug free_area struct: x/30x &free_area
- Invert the bit for the block that is being subdivided AND for the block truly/entirely allocated.

# 6.3 - Free Pages

- The API for the freeing of pages is a lot simpler and exists to help remember the order of the block to free
    - This because one disadvantage of a buddy allocator is that the caller has to remember the size of the original allocation
    - Save it on a header in the block
- API
    - `void __free_pages(struct page *page, unsigned int order)` - Free an order number of pages from the given page
    - `void __free_page(struct page *page)` - Free a single page
    - `void free_page(void *addr)` - Free a page from the given virtual address
- The principal function for freeing pages is **__free_pages_ok()**
![Call Graph: __free_pages()](https://www.kernel.org/doc/gorman/html/understand/understand-html032.png)
- When a buddy is freed, coalesce the buddies together immediately if possible
    - To detect if the buddies can be merged or not, check the bit corresponding to the affected pair of buddies in free_area→map
    - As one buddy has just been freed by this function, it is obviously known that at least one buddy is free
    - If the bit in the map is 0 after toggling (call **MARK_USED**), we know that the other buddy must also be free because if the bit is 0, it means both buddies are either both free or both allocated
    - If both are free, they may be merged
- Calculating the address of the buddy
    - As the allocations are always in blocks of size 2<sup>k</sup>, the address of the block, or at least its offset within **zone_mem_map** will also be a multiple of 2<sup>k</sup>
    - The end result is that there will always be at least k number of zeros to the right of the address
    - To get the address of the buddy, the *kth* bit from the right is examined
    - The buddy will have this bit flipped
        - ```
            __find_buddy_pfn(unsigned long page_pfn, unsigned int order)
            {
	            return page_pfn ^ (1 << order);
            }
    	- ```
        	mask = (~0 << k) 
        	imask = 1 + ~mask
        	imask = -mask = 1 + ~mask
       		```
- Once the buddy is merged, it is removed for the free list and the newly coalesced pair moves to the next higher order to see if it may also be merged
- When merging buddies, invert the bit for the new merged block, independent of the option chosen in the allocation

