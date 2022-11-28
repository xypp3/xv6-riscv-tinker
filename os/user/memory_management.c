#include "memory_management.h"

#define DEBUG

static Header *base_head = NULL; // empty starting list of free and used blocks

Header *more_mem(int size){
    // user requested size + ( 2*sizeof(Header) ) is assumed
    int n_pages = ( ( size / PAGE_SIZE ) + 2 ) * PAGE_SIZE; 
        // Min number of FULL pages needed for storing size + 1 additional page 
            // (incase size is almost the size of the next closest page)

    // sbrk
    Header *get_mem = (Header *) sbrk(n_pages);
    
    #ifdef DEBUG
        printf("--%x--\n", get_mem);
    #endif

    // sbrk check (not enough physical memory)
    if (((void *) get_mem) == ((void *)-1))
        return NULL;

    // find end of list
    Header *iter_ptr = base_head;
    while (iter_ptr->h.next_head != NULL)
        iter_ptr = iter_ptr->h.next_head;
    // cast new block and do basic operations
    // Header *gotten_mem = (Header *) (get_mem);
    iter_ptr->h.next_head = get_mem;
    get_mem->h.size = n_pages;

    // if sbrk block size == free block size
    if (get_mem->h.size == size){
        get_mem->h.next_head = NULL;
        
        return get_mem; // return used_block with head 

    }else // else trunkate free block (cut into free and used parts)
        return butcher(get_mem, size-sizeof(Header), NULL);
}

// cut big block into free and used blocks AND connect them to the list
    // assumes big_block->h.size > sizeof(Header)*2 + user requested size
    // assumes using_size == user requested size + sizeof(Header)
Header *butcher(Header *big_block, int using_size, void * next_ptr){
        void *used_block = ((void *) big_block); // cast to allow for pointer arithmetic (+1 == +1 bit)
        used_block      += (big_block->h.size) - (using_size); // move to new block
        big_block->h.size -= using_size; // change free block size
        big_block->h.next_head = ((Header *) used_block); // connect last free block to last used block

        // links with next block in list, which could be the end of the list
        if (next_ptr == NULL)
            ((Header *)used_block)->h.next_head   = NULL; 
        else
            ((Header *)used_block)->h.next_head   = ((Header *) next_ptr);
        
        ((Header *)used_block)->h.size        = using_size - sizeof(Header);
        ((Header *)used_block)->h.is_free     = FALSE;
        
        // if next ptr is null then it assumes that it's a call made by more_mem() 
            // and you want to combine new block with previous free blocks if need be
        if(next_ptr == NULL){
            big_block->h.is_free = FALSE; // THIS GETS CHANGED BY _free()
            _free(big_block + 1); // to potentially connect free section with previous free block
                // must free the body of big_block and not head
        }

        return ((Header *) used_block); // return used_block with head
}

void *_malloc(int size){
    // sanity check
    if( !( 0 < size  && size <= ( __INT32_MAX__ - 2*sizeof(Header) ) ) ){
        return NULL;
    }
    // start malloc from here

    // if base_head == null

    if(base_head == NULL){
        // do setup
        base_head = (Header *) sbrk(sizeof(Header));
        // no more memory available
        if (((void *) base_head) == ((void *)-1))
            return NULL;

        base_head->h.next_head    = NULL;
        base_head->h.is_free      = FALSE;
        base_head->h.size         = sizeof(Header);

    }

    Header *iter_ptr    = base_head->h.next_head;
    Header *best_fit    = base_head;
    best_fit->h.size      = __INT32_MAX__;
    // main loop (for every element on the list)
    while(iter_ptr != NULL){
        if(iter_ptr->h.is_free == TRUE){
            if(iter_ptr->h.size >= (size + (2*sizeof(Header)))
                && best_fit->h.size > iter_ptr->h.size
                ){
                    best_fit            = iter_ptr;
                    best_fit->h.size      = iter_ptr->h.size;
                    best_fit->h.next_head = iter_ptr->h.next_head;
                }
            #ifdef DEBUG
                printf("free ptrs: %x\n", iter_ptr);
            #endif
        }

        iter_ptr = iter_ptr->h.next_head;
    }

    // if end of list reached and best_fit not re-assigned
    if(best_fit == base_head)
        return more_mem(size + (2*sizeof(Header))) + 1; // returns block without head

    else
        return butcher(best_fit, size + sizeof(Header), (void *) best_fit->h.next_head) + 1; // returns block without head
}


void  _free(void *ptr){
    if(ptr == NULL || base_head == NULL){
        #ifdef DEBUG
            printf("ptr is null or base_head is not set\n");
        #endif
        return;
    }

    // get ptr head
    Header *to_free = ((Header *) ptr) - 1; // should have size n stuff

    // if ptr is already free 
    if (to_free->h.is_free == TRUE){
        #ifdef DEBUG
            printf("already freed\n");
        #endif

        return;
    }

    // iterate through 
    Header *prev_ptr = base_head;
        // find the to_free pointer in the linked list (implied it cannot be the head)
    while(prev_ptr->h.next_head != to_free && prev_ptr->h.next_head != NULL){
        prev_ptr = prev_ptr->h.next_head;
        #ifdef DEBUG
            printf("Is free: %d, %x\n", prev_ptr->h.is_free, prev_ptr);
        #endif
    }

    // if to_free is not in heap but is a valid Header *
    if (prev_ptr == NULL){
        #ifdef DEBUG
            printf("ptr is not in heap or ptr == base_head\n");
        #endif

        return;
    }

    // found and valid free block so free
    to_free->h.is_free = TRUE;
    
    // merge free block with previous free block
    if(prev_ptr->h.is_free == TRUE){
        prev_ptr->h.next_head = to_free->h.next_head;
        prev_ptr->h.size     += sizeof(Header) + to_free->h.size;
        to_free = prev_ptr; // set free block to left most block
        
    }

    // if it isn't final block and you can merge with next block
    if(to_free->h.next_head != NULL 
        && to_free->h.next_head->h.is_free == TRUE
        ){ 
        to_free->h.size      += to_free->h.next_head->h.size + sizeof(Header); // increase size of to_free with right merged free block
        to_free->h.next_head  = to_free->h.next_head->h.next_head; // link to_free with right.next_head
    }

}

#ifdef DEBUG

    void printHeap(){
        Header *iter_ptr = base_head;
        printf("%x ", iter_ptr);
        while(iter_ptr != NULL){
            printf("--> %x:%d ", iter_ptr, iter_ptr->h.is_free);
            iter_ptr = iter_ptr->h.next_head;
        }
        printf("\n");
    }

    void free_test(){

        // early exit conditions
        void * tf = (void *) 0xffff;
        void * tf2 = NULL;
        _free(tf);  // err: head not defined
        _free(tf2); // err: ptr == NULL

        // merge tests
        void* merge1 = _malloc(32);
        void* merge2 = _malloc(32);
        void* merge3 = _malloc(32);
        void* merge4 = _malloc(8000);

        // right merge 
        printf("Right merge: %d\n", 1);
        _free(merge1);
        printHeap();
        _free(merge2);
        printHeap();
        // left merge
        printf("Left merge: %d\n", 1);
        _free(merge4);
        printHeap();
        // double merge
        printf("Both merge: %d\n", 1);
        _free(merge3);
        printHeap();


    }

    int main (int argc, void **argv){

        printf("runs\n");
        


        free_test();
        
        

        // test more mem
        // base_head = (Header *) sbrk(sizeof(Header));
        // base_head->h.next_head = NULL;
        // printf("header size: %x\n", sizeof(Header));
        // printf("header ptr size: %x\n", sizeof(Header *));
        // printf("complete 1, %x\n", more_mem(116));
        // printf("complete 2, %x\n", more_mem(0));
        // for(int i = 0; i < 1000; i++){
        //     Header *n = more_mem(1000000);
        //     if(n == NULL)
        //         break;
        //     printf("complete 3:%d, size:%d, next_size:%d, addr:%x\n", i, sizeof(n), sizeof(n->h.next_head), n );
        // }
        // printf("complete 4, %x\n", more_mem(1));
        

        return 0;
    }

#endif

// .c file TODOs
// 1. _free() doesn't have a way to verify void* can be cast to Header*
// 2. malloc() won't fit a block that is sizeof(Header + BODY) as algo needs sizeof(2*Header + BODY)