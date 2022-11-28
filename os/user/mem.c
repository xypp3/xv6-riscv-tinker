#include "mem.h"

static Header *base_head = NULL; // empty starting list of free and used blocks

Header *more_mem(int size){
    // user requested size + ( 2*sizeof(Header) ) is assumed
    int n_pages = ( ( size / PAGE_SIZE ) + 2 ) * PAGE_SIZE; 
        // Min number of FULL pages needed for storing size + 1 additional page 
            // (incase size is almost the size of the next closest page)

    // sbrk
    Header *get_mem = (Header *) sbrk(n_pages);
    printf("--%x--", get_mem);
    // sbrk check (not enough physical memory)
    if (((void *) get_mem) == ((void *)-1))
        return NULL;

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

    }else // else trunkate free block (cut into free and used parts)
        return butcher(get_mem, size, NULL);
}

// cut big block into free and used blocks AND connect them to the list
    // assumes big_block->size > sizeof(Header)*2 + user requested size
    // assumes using_size == user requested size + sizeof(Header)
Header *butcher(Header *big_block, int using_size, void * next_ptr){
    // printf("here\n");
        void *used_block = ((void *) big_block); // cast to allow for pointer arithmetic (+1 == +1 bit)
        // printf("here2\n");
        used_block      += (big_block->size) - (using_size); // move to new block
        // printf("here3\n");
        big_block->size -= using_size - sizeof(Header); 
            // free_block.size == sizeof(Header) + sizeof(BODY) && sizeof(BODY) == big_block.size - using_size
        // big_block->size -= (using_size); // cut big block size by used_size
        // printf("here4\n");
        big_block->next_head = ((Header *) used_block); // connect last free block to last used block
        // printf("here5\n");
        // links with next block in list, which could be the end of the list
        if (next_ptr == NULL)
            ((Header *)used_block)->next_head   = NULL; 
        else
            ((Header *)used_block)->next_head   = ((Header *) next_ptr);
        
        // printf("here6\n");
        ((Header *)used_block)->size        = using_size;
        // printf("here 7\n");
        ((Header *)used_block)->is_free     = FALSE;
        
        big_block->is_free = FALSE; // THIS GETS CHANGED BY 
        _free(big_block + 1); // to potentially connect free section with previous free block
            // must free the body of big_block and not head

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

        base_head->next_head    = NULL;
        base_head->is_free      = FALSE;
        base_head->size         = sizeof(Header);

    }

    Header *iter_ptr    = base_head->next_head;
    Header *best_fit    = base_head;
    best_fit->size      = __INT32_MAX__;
    // main loop
    // printf("")
    while(iter_ptr != NULL){
        if(iter_ptr->is_free == TRUE){
            if(iter_ptr->size >= (size + (2*sizeof(Header)))
                && best_fit->size > iter_ptr->size
                ){
                    best_fit            = iter_ptr;
                    best_fit->size      = iter_ptr->size;
                    best_fit->next_head = iter_ptr->next_head;
                }
        }

        iter_ptr = iter_ptr->next_head;
    }

    // if end of list reached and best_fit not re-assigned
    if(best_fit == base_head)
        return more_mem(size + (2*sizeof(Header))) + 1; // returns block without head

    else
        return butcher(best_fit, size + sizeof(Header), (void *) best_fit->next_head) + 1;
}


void  _free(void *ptr){
    if(ptr == NULL || base_head == NULL){
        printf("ptr is null or base_head is not set\n");
        return;
    }

    // get ptr head
    Header *to_free = ((Header *) ptr) - 1; // should have size n stuff

    // if ptr is already free 
    if (to_free->is_free == TRUE)
        return;

    // iterate through 
    Header *prev_ptr = base_head;
        // find the to_free pointer in the linked list (implied it cannot be the head)
    while(prev_ptr->next_head != to_free && prev_ptr->next_head != NULL) 
        prev_ptr = prev_ptr->next_head;

    // if to_free is not in heap but is a valid Header *
    if (prev_ptr == NULL){
        printf("ptr is not in heap or ptr == base_head");
        return;
    }
    
    // merge free block with previous free block
    if(prev_ptr->is_free == TRUE){
        prev_ptr->next_head = to_free->next_head;
        prev_ptr->size     += sizeof(Header) + to_free->size; 
    }else
        to_free->is_free = TRUE; // will be start of merged free block

    // if it isn't final block and you can merge with next block
    if(to_free->next_head != NULL 
        && to_free->next_head->is_free == TRUE
        ){ 
        to_free->size      += to_free->next_head->size + sizeof(Header); // increase size of to_free with right merged free block
        to_free->next_head  = to_free->next_head->next_head; // link to_free with right.next_head
    }
}


int main (int argc, void **argv){

    printf("runs\n");
    // test free
    void * tf = (void *) 0xffff;
    void * tf2 = NULL;
    _free(tf);  // err: head not defined
    _free(tf2); // err: ptr == NULL


    // void * tf3 = (Header *) base_head;
    // _free(tf3); // ptr not in heap or not HEADER convertable

    void* test = _malloc(32);
    printf("test: %x\n", test);
    // _free(test);
    void* test1 = _malloc(32);
    void* test2 = _malloc(32);
    void* test3 = _malloc(8000);


    
    printf("head: %x, next: %x\n", base_head, base_head->next_head);
    printf("test1: %x\n", test1);
    printf("test2: %x\n", test2);
    printf("test3: %x\n", test3);
    

    // test more mem
    // base_head = (Header *) sbrk(sizeof(Header));
    // base_head->next_head = NULL;
    // printf("header size: %x\n", sizeof(Header));
    // printf("header ptr size: %x\n", sizeof(Header *));
    // printf("complete 1, %x\n", more_mem(116));
    // printf("complete 2, %x\n", more_mem(0));
    // for(int i = 0; i < 1000; i++){
    //     Header *n = more_mem(1000000);
    //     if(n == NULL)
    //         break;
    //     printf("complete 3:%d, size:%d, next_size:%d, addr:%x\n", i, sizeof(n), sizeof(n->next_head), n );
    // }
    // printf("complete 4, %x\n", more_mem(1));
    

    return 0;
}


// .h file TODOs
// 1. How do you determine the size of a struct?
//  a. should I make it a union with size long to stabilize size?

// .c file TODOs
// 1. _free() doesn't have a way to verify void* can be cast to Header*