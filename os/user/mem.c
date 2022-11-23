#include "mem.h"

static Header *head = NULL; // empty starting list of free blocks

Header *more_mem(unsigned long size){
    unsigned long n_pages_requested = (size/PAGE_SIZE) + 1; // request at least one page as it is smallest addressable block of mem

    Header *appended_block = (Header *) sbrk(n_pages_requested);
    if(appended_block == NULL)
        return NULL;

    // find end of list and append block
    Header *curr_ptr = head;
    printf("3:%x\n", curr_ptr);
    while(curr_ptr->nextHeader != NULL){
        curr_ptr = curr_ptr->nextHeader;
    }
    // curr_ptr == last element in list
    curr_ptr->nextHeader       = appended_block;
    appended_block->nextHeader = NULL; // loop list round
    appended_block->size       = ( PAGE_SIZE * n_pages_requested ) - size;

    // printf("1:%x\n", appended_block->nextHeader);
    // printf("2:%x\n", curr_ptr);

    return appended_block; //start of new block (mem addr. includes header)
}

void *_malloc(int size){
    // sanity check
    if(size <= 0){
        return NULL;
    }
    // start malloc from here
    Header *prev_ptr;
    Header *iter_ptr;
    // long to provent overflow of MAX_INT + sizeof(Header)
    unsigned long requested_size = ((unsigned int) size) + sizeof(Header);  

    // if free list not created, then create it
    if( head == NULL){
        head = (Header *) sbrk(sizeof(Header));
        // if more_mem errors, return NULL
        if (head == NULL)
            return NULL;

        head->nextHeader = NULL; // head starts off as circular  
        head->size       = sizeof(Header); 
    }


    prev_ptr = head;
    iter_ptr = head->nextHeader; // start at head

    while(iter_ptr != NULL){
        // printf("s:%d, req:%d\n", iter_ptr->size, requested_size);
        if(iter_ptr->size >= requested_size){
            if(iter_ptr->size == requested_size){
                // printf("here2");
                // skip over iter_ptr in free list
                prev_ptr->nextHeader = iter_ptr->nextHeader;
            }else{
                // allocate tail end to not have to move prev_ptr->nextHeader to a different pointer
                // printf("here3: %d", iter_ptr->size);
                iter_ptr->size -= requested_size;
                // printf("here4: %d", iter_ptr->size);
                iter_ptr += iter_ptr->size; // move to start of allocated mem block
                // printf("here5: %x", iter_ptr);
                // iter_ptr->size = requested_size;
                // printf("here6");
                // todo ^^^ see if not having this causes an error when freeing most likely it will appear
            }

            return (void *)(iter_ptr + 1); // return block without head
        }

        // move through the linked list by one item
        // printf("hi: %x\n", iter_ptr->size);
        prev_ptr = iter_ptr;
        iter_ptr = iter_ptr->nextHeader;
        
    }

    // printf("boooo\n");
    // only called if large enough block not found
    void *block = more_mem(requested_size);
    if (block == NULL)
        return NULL;
    else 
        return (void *)(block + 1); // return block without head
}


void  _free(void *ptr){
    // todo: check if ptr has been freed before
    if(ptr == NULL)
        return;

    
}


int main (int argc, void **argv){

    printf("runs\n");

    void* test = _malloc(200);
    // todo test free here
    void* test1 = _malloc(PAGE_SIZE-(200+32) );
    // void* test2 = _malloc(5000);
    // void* test3 = _malloc(100);


    
    printf("head: %x, next: %x\n", head, head->nextHeader);
    printf("test: %x\n", test);
    printf("test1: %x\n", test1);
    // printf("test2: %x\n", test2);
    // printf("test3: %x\n", test3);


    return 0;
}