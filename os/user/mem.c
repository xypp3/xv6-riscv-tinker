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
    if (((void *) get_mem) == ((void *)-1)){
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

    }else // else trunkate free block (cut into free and used parts)
        return butcher(get_mem, size, ((Header *) NULL));
}

// cut big block into free and used blocks AND connect them to the list
    // assumes big_block->size > sizeof(Header)*2 + user requested size
    // assumes using_size == user requested size + sizeof(Header)
Header *butcher(Header *big_block, int using_size, Header * next_ptr){
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
        ((Header *)used_block)->next_head   = next_ptr; // links with next block in list which could be the end of the list i.e NULL
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



    return NULL;
}


void  _free(void *ptr){
    if(ptr == NULL || base_head == NULL){
        printf("ptr is null or base_head is not set");
        return;
    }

    // get ptr head
    Header *to_free = ((Header *) ptr) - 1; // should have size n stuff
    
    // if ptr is already free 
    if (to_free->is_free == TRUE)
        return;
    
    // iterate through 
    Header *prev_ptr = base_head;
    while(prev_ptr->next_head != to_free) // find the to_free pointer in the linked list
        prev_ptr = prev_ptr->next_head;

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
    _free(tf);
    _free(tf2);

    // test more_mem
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
    

    // test more mem
    printf("header size: %x\n", sizeof(Header));
    printf("header ptr size: %x\n", sizeof(Header *));
    printf("complete 1, %x\n", more_mem(116));
    printf("complete 2, %x\n", more_mem(0));
    for(int i = 0; i < 1000; i++){
        Header *n = more_mem(1000000);
        if(n == NULL)
            break;
        printf("complete 3:%d, size:%d, next_size:%d, addr:%x\n", i, sizeof(n), sizeof(n->next_head), n );
    }
    printf("complete 4, %x\n", more_mem(1));
    

    return 0;
}


// header file TODOs
// 1. How do you determine the size of a struct?
//  a. should I make it a union with size long to stabilize size?
