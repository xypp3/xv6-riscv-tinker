#include "kernel/types.h"
#include "user/user.h"

// define vars and data structs
#define PAGE_SIZE 4096
#define NULL      (void *) 0

typedef struct h {
    struct h *nextHeader;
    unsigned size; // size of this memory block
} Header;

// defined methods
int main (int argc, void **argv);
void *_malloc(int size);
void  _free(void *ptr);
