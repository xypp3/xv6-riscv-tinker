#include "kernel/types.h"
#include "user/user.h"

// define vars and data structs
#define PAGE_SIZE   4096
#define NULL        (void *) 0
#define TRUE        1
#define FALSE       0

// linked list struct
typedef union header{
    struct h
    {
        union header *next_head;
        char is_free;
        unsigned size;
    }h;
    
    uint16 sizeAlignment; // not used, just to set consistant size of header

} Header;


// defined methods
void *_malloc(int size);
void  _free(void *ptr);
Header *more_mem(int size);
Header *butcher(Header *big_block, int using_size, void * next_ptr);
