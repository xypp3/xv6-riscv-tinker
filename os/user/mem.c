#include "mem.h"

static Header *base_head = NULL; // empty starting list of free and used blocks

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
    if (size == n_pages_in_bytes){
        curr_ptr->next_head        = appended_block; // link to end of list
        appended_block->next_head  = NULL; // make it new end of list
        appended_block->size       = size; // set size of (new block(s) - malloc_used_block_size)
    }else{
        printf("1:append_ptr: %x, sbrk: %d\n", appended_block, n_pages_in_bytes);
        printf("2: c-%x, a-%x, as-%d, cs-%d\n", curr_ptr, appended_block, appended_block->size, size);
        // appended_block             += appended_block->size; // move to malloc_used_block 
        // appended_block->size        = size - sizeof(Header); // set size of  

        // for (int i = 0; i < n_pages_in_bytes; i++){
        //     int add = i/2;
        //     (appended_block+add)->size = 1;
        //     printf("%d-> %x\n", i, appended_block+add);
        // }
        

        Header *used_block = (Header *) ( appended_block->size - size + ((void *)appended_block) );
        // // printf("%x\n", appended_block->size+appended_block);
        printf("used ptr: %x, used size: %d\n", used_block, appended_block->size-size);
        // (appended_block+new_size)->size = size;

        printf("4: c-%x, a-%x, as-%d, cs-%d\n", curr_ptr, appended_block, appended_block->size, size);

        // set free
        // used_block->is_free     = FALSE;
        printf("pls\n");
        appended_block->is_free = TRUE;
        printf("here!!!!!!\n");
    }
    
    

    return appended_block; //start of malloc_used_block
}

void *_malloc(int size){
    // sanity check
    if( !( 0 < size  && size <= ( __INT32_MAX__ - sizeof(Header) ) ) ){
        return NULL;
    }
    // start malloc from here
    Header * iter_ptr;

    unsigned int requested_size = size + sizeof(Header);  

    // if list not created, then create it
    if( base_head == NULL){
        base_head = (Header *) sbrk(sizeof(Header));
        // if more_mem errors, return NULL
        if (base_head == NULL)
            return NULL;

        base_head->next_head    = NULL; // base_head starts off as circular  
        base_head->size         = sizeof(Header);
        base_head->is_free      = FALSE;
    }

    iter_ptr = base_head->next_head; // start at base_head
    printf("%x\n", iter_ptr);
    while (iter_ptr != NULL && iter_ptr->is_free == TRUE){
        if(iter_ptr->size >= requested_size)
            break; // as iter_ptr is free block of right size
        iter_ptr = iter_ptr->next_head; // increment through list
        printf("hi\n");
    }
 
    if(iter_ptr == NULL){
        return (void *) (more_mem(requested_size));
    }else{ // found a free block of big ENOUGH
        printf("1: %d\n", iter_ptr->size);
        iter_ptr->size     -= requested_size;
        printf("2\n");
        Header *used_block  = ((void *) iter_ptr) + iter_ptr->size;
        printf("here: %x", used_block);
        // used_block->is_free = FALSE;
        // used_block->size    = requested_size;
        // // unlink and link the list (iter_ptr --> used_block --> iter_ptr.next_head)
        // used_block->next_head   = iter_ptr->next_head; // 
        // iter_ptr->next_head     = used_block;
        
        // return (void *) (used_block + 1);
    }

    return NULL;
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