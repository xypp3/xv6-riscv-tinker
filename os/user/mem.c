#include "mem.h"

static Header *base == NULL; // empty starting list

void* more_mem(unsigned long size){
    unsigned long n_pages_requested = (size/PAGE_SIZE) + 1; // request at least one page as it is smallest addressable block of mem

    Header* appended_block = (Header *) sbrk(n_pages_requested);
    if(appended_block == NULL)
        return NULL;

    // find end of list and append block
    Header *curr_ptr = base.nextHeader;
    Header *prev_ptr;
    while(curr_ptr != base){
        prev_ptr = iter_ptr;
        curr_ptr = iter_ptr.nextHeader;
    }
    // append to prev_ptr
    prev_ptr = appended_block;
    appended_block->nextHeader = base; // loop list round
    appended_block->size`      = n_pages_requested;
    // free all used ptr
    _free(prev_ptr);
    _free(curr_ptr);
    _free(appended_block);
}

void* _malloc(int size){
    // sanity check
    if(size <= 0){
        return NULL;
    }
    // start malloc from here
    Header* prev_ptr = base.nextHeader;
    Header* iter_ptr = prev_ptr->nextHeader;
    // long to provent overflow of MAX_INT + sizeof(Header)
    unsigned long requested_size = ((unsigned int) size) + sizeof(Header);  

    // if free list not created, then create it
    if( base == NULL){
        base->nextHeader = base; // base starts off as circular  
        base->size       = 0; // size 0 as body of header is empty
        void* err = more_mem(requested_size); // get more mem
        // if more_mem errors, return NULL
        if (err == NULL)
            return NULL;    
    }

    while(free_ptr != iter_ptr){

        if(iter_ptr.size >= requested_size){
            if(iter_ptr.size == requested_size){
                // get iter_ptr out of free list
                prev_ptr.nextHeader = iter_ptr.nextHeader;
            }else{
                // allocate tail end to not have to move prev_ptr.nextHeader to a different pointer
                iter_ptr.size -= requested_size;
                iter_ptr += iter_ptr.size; // move to start of allocated mem block
                iter_ptr.size = requested_size;
            }
            
            return (void *)(iter_ptr + 1); // return block without head
        }

        // move through the linked list by one item
        prev_ptr = iter_ptr;
        iter_ptr = iter_ptr->nextHeader;
    }
    // if not enough memory owned by malloc get more memory

}


void  _free(void* ptr);{
    // todo: check if ptr has been freed before
    if(ptr == NULL)
        return;

    
}


int main (int argc, void **argv){

    printf("runs\n");

    return 0;
}