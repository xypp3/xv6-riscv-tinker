#include "kernel/types.h"
#include "user/user.h"

// define vars and data structs
#define PAGE_SIZE   4096
#define NULL        (void *) 0
#define TRUE        1
#define FALSE       0

// linked list struct
typedef struct h {
    struct h *next_head;
    char is_free; //boolean 
    unsigned size; // size of this memory block
} Header;

// defined methods
void *_malloc(int size);
void  _free(void *ptr);
Header *more_mem(int size);
Header *butcher(Header *big_block, int used_size, int big_block_size);
