

// Allocate a single page and return a struct address
struct page * alloc_page(unsigned int gfp_mask);

// Allocate 2^order number of pages and returns a struct page
struct page * alloc_pages(unsigned int gfp_mask, unsigned int order);

// Allocate a single page, zero it and return a virtual address
unsigned long get_free_page(unsigned int gfp_mask);

// Allocate a single page and return a virtual address
unsigned long __get_free_page(unsigned int gfp_mask);

// Allocate a 2^order number of pages and return a virtual address 
unsigned long __get_free_pages(unsigned int gfp_mask, unsigned int order);
