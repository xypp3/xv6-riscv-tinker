#include "mem.h"

static Header *base_head = NULL; // empty starting list of free blocks

Header *more_mem(int size){
    unsigned int n_pages_in_bytes = ((size/PAGE_SIZE) + 1) * PAGE_SIZE; // request at least one page as it is smallest addressable block of mem

    Header *appended_block = (Header *) sbrk(n_pages_in_bytes);
    if(appended_block == NULL)
        return NULL;

    // find end of list and append block
    Header *curr_ptr = base_head;
    printf("3:%x\n", curr_ptr);
    while(curr_ptr->next_head != NULL){
        curr_ptr = curr_ptr->next_head;
    }
    // curr_ptr == last element in list
    curr_ptr->next_head        = appended_block; // link to end of list
    appended_block->next_head  = NULL; // make it new end of list
    appended_block->size        = n_pages_in_bytes; // set size of (new block(s) - malloc_used_block_size)
    printf("1:append_ptr: %x, sbrk: %d\n", appended_block, n_pages_in_bytes);
    printf("2: c-%x, a-%x, as-%d, cs-%d\n", curr_ptr, appended_block, appended_block->size, size);
    // appended_block             += appended_block->size; // move to malloc_used_block 
    // appended_block->size        = size - sizeof(Header); // set size of  
    for (int i = 0; i < n_pages_in_bytes; i++){
        int add = i/2;
        (appended_block+add)->size = 1;
        printf("%d-> %x\n", i, appended_block+add);
    }
    

    Header *used_block = appended_block->size-size+appended_block;
    // printf("%x\n", appended_block->size+appended_block);
    printf("used ptr: %x, used size: %d\n", used_block, appended_block->size-size);
    used_block->size = size;

    printf("4: c-%x, a-%x, as-%d, cs-%d\n", curr_ptr, appended_block, appended_block->size, size);

    return appended_block; //start of malloc_used_block
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

    unsigned int requested_size = size + sizeof(Header);  

    // if free list not created, then create it
    if( base_head == NULL){
        base_head = (Header *) sbrk(sizeof(Header));
        // if more_mem errors, return NULL
        if (base_head == NULL)
            return NULL;

        base_head->next_head = NULL; // base_head starts off as circular  
        base_head->size       = sizeof(Header); 
    }


    prev_ptr = base_head;
    iter_ptr = base_head->next_head; // start at base_head

    while(iter_ptr != NULL){
        // printf("s:%d, req:%d\n", iter_ptr->size, requested_size);
        if(iter_ptr->size >= requested_size){
            if(iter_ptr->size == requested_size){
                // printf("here2");
                // skip over iter_ptr in free list
                prev_ptr->next_head = iter_ptr->next_head;
            }else{
                // allocate tail end to not have to move prev_ptr->next_head to a different pointer
                printf("here3: %d, %d\n", iter_ptr->size, requested_size);
                iter_ptr->size -= requested_size;
                printf("here4: %d, %d\n", iter_ptr->size, requested_size);
                // iter
                // Header *used = iter_ptr + iter_ptr->size + 1; // move to start of allocated mem block
                // printf("here5: %x", iter_ptr);
                // used->size = requested_size;
                printf("here6");
                // todo ^^^ see if not having this causes an error when freeing most likely it will appear
            }

            return (void *)(iter_ptr + 1); // return block without head
        }

        // move through the linked list by one item
        // printf("hi: %x\n", iter_ptr->size);
        prev_ptr = iter_ptr;
        iter_ptr = iter_ptr->next_head;
        
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
    // void* test1 = _malloc(PAGE_SIZE-(200+32) );
    // void* test2 = _malloc(50000);
    void* test3 = _malloc(100);


    
    printf("head: %x, next: %x\n", base_head, base_head->next_head);
    printf("test: %x\n", test);
    // printf("test1: %x\n", test1);
    // printf("test2: %x\n", test2);
    printf("test3: %x\n", test3);


    return 0;
}