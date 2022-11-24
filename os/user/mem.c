#include "mem.h"

static Header *base_head = NULL; // empty starting list of free and used blocks

Header *more_mem(int size){
    // user requested size + sizeof(Header) is assumed
    int n_pages = ( ( size / PAGE_SIZE ) + 1 ) * PAGE_SIZE; // Min number of FULL pages needed for storing size

    // sbrk
    Header *get_mem = (Header *) sbrk(n_pages);
    printf("--%x--", get_mem);
    // sbrk check
    if (((void *) get_mem) == ((void *)-1)){
        // printf("TOO MANY PAGES");
        return NULL;
    }

    // find end of list
    Header *iter_ptr = base_head;
    while (iter_ptr->next_head != NULL)
        iter_ptr = iter_ptr->next_head;
    // cast new block and do basic operations
    // Header *gotten_mem = (Header *) (get_mem);
    iter_ptr->next_head = get_mem;
    get_mem->size = n_pages;

    // if sbrk block size == free block size
    if (get_mem->size == size){
        get_mem->next_head = NULL;
        
        return get_mem; // return used_block with head 

    }else{ // else shrink free block (split into free and used parts)
        printf("here\n");
        get_mem->size -= (size); // n_pages - size
        printf("here2\n");
        void *used_block = ((void *) get_mem); // cast to allow for pointer arithmetic
        printf("here3\n");
        used_block += (n_pages) - (get_mem->size); // move to new block
        printf("here4\n");
        get_mem->next_head = ((Header *) used_block); // connect last free block to last used block
        printf("here5\n");
        ((Header *)used_block)->next_head = NULL; // connect last used block to end
        printf("here6\n");

        // todo: free(gotten_mem) to potentially connect free section with previous free block

        return ((Header *) used_block); // return used_block with head
    }
}

void *_malloc(int size){
    // sanity check
    if( !( 0 < size  && size <= ( __INT32_MAX__ - sizeof(Header) ) ) ){
        return NULL;
    }
    // start malloc from here



    return NULL;
}


void  _free(void *ptr){
    // todo: check if ptr has been freed before
    if(ptr == NULL)
        return;

    
}


int main (int argc, void **argv){

    printf("runs\n");
    base_head = (Header *) sbrk(sizeof(Header));
    base_head->next_head = NULL;

    // void* test = _malloc(200);
    // // todo test free here
    // // void* test1 = _malloc(PAGE_SIZE-(200+32) );
    // // void* test2 = _malloc(50000);
    // void* test3 = _malloc(100);


    
    // printf("head: %x, next: %x\n", base_head, base_head->next_head);
    // printf("test: %x\n", test);
    // // printf("test1: %x\n", test1);
    // // printf("test2: %x\n", test2);
    // printf("test3: %x\n", test3);
    
    printf("header size: %x\n", sizeof(Header));
    printf("header ptr size: %x\n", sizeof(Header *));
    printf("complete 1, %x\n", more_mem(116));
    // printf("complete 2, %x\n", more_mem(0));
    printf("complete 3, %x\n", more_mem(9000));
    printf("complete 4, %x\n", more_mem(1));
    


    return 0;
}