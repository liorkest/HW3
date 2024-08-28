#include "customAllocator.h"
#include <stdio.h>
#include <string.h>

Block* block_list = nullptr;


void print_block(Block* block){
    printf("Block: %p\n", (void*)block);
    printf("Size: %lu bytes\n", block->size);
    printf("Free: %s\n", block->free? "Yes" : "No");
    printf("Next: %p\n", (void*)block->next);
}

void print_list(){
    Block* block;
    for(block = block_list; block != nullptr; block = block->next){
        printf("%p (%lu, %s) -> ", (void*)block, block->size, block->free? "free" : "full");
    }
    printf("NULL\n");
}
void merge_free_blocks(){
    // Merge adjacent free blocks
    Block* block = block_list;
    while (block != NULL && block->next != NULL)
    { 
        if(block->free && block->next->free)
        {
            //printf("Merging free blocks!\n");
            block->size = block->size + sizeof(Block) + block->next->size; // size of merged block
            block->next = block->next->next;
        }
        else
            block = block->next;
    }
}

void rm_block_from_linked_list(Block* b){
    //printf("Removing block %p from list\n", (void*)b);
    print_block(b);   
    if (b == block_list) {
        block_list = block_list->next;
    } else {
        Block* prev = block_list;
        while(prev->next!= b) {
            prev = prev->next;
        }
        prev->next = b->next;
    }
}

void shrink_heap(){

    // check if last block is free
    Block* last_block = nullptr, *curr = block_list;
    for(; curr->next != nullptr; curr = curr->next);
    last_block = curr;

    // Check if the last block is free
    if (last_block->free == true) {
        //printf("removing last block\n");
        // Check if the current break can be reduced
        void* program_break = sbrk(0); // Get current program break
        void* block_end = (void*)((char*)last_block + sizeof(Block) + last_block->size);

        if (block_end == program_break) {
            rm_block_from_linked_list(last_block);
            // Reduce the program break to release the memory back to the OS
            if (sbrk((-1)*(sizeof(Block) + last_block->size)) == SBRK_FAIL) {
                printf("sbrk failed; could not reduce the break\n");
                return;
            }
            
        }
    }
}

void add_block_to_linked_list(Block* new_block){
    //printf("Adding block with size %lu to list\n", new_block->size);
    if(block_list == nullptr) {
        //printf("Initialized block list\n");
        block_list = new_block;
    } else {
        // Add the new block to the end of the linked list
        Block* block;
        for(block = block_list; block->next != NULL; block = block->next);
        block->next = new_block;
    }
}

void* customMalloc(size_t size) {
    // Allocate memory using your custom allocator
    // Return a pointer to the allocated memory
    
    if (size<=0){
        printf("<malloc error>: passed nonpositive size\n");
        return NULL;
    }
    int aligned_size = ALIGN_TO_8BIT(size);
    Block* block = findFreeBlock(aligned_size);
    if (block == NULL) {
        printf("<malloc error>: out of memory\n");
        return NULL;
    }


    return block;
}

void customFree(void* ptr) {
    if (!ptr) {
        printf("<free error>: passed null pointer\n");
        return;
    }
    Block* block = (Block*)ptr - 1; // get pointer to the block header
    
    if (block->free == true) {
        printf("<free error>: trying to free already freed memory\n");
        return;
    }
    block->free = 1;
    // merge adjacent free blocks
    merge_free_blocks();
    // reduce pbrk if possible
    shrink_heap();
}

void* customCalloc(size_t nmemb, size_t size){
    // Allocate and initialize memory using your custom allocator
    // Return a pointer to the allocated memory
    size_t total_size = nmemb * size;
    if(nmemb <= 0 || size <= 0) {
        printf("<calloc error>: passed nonpositive size\n");
        return NULL;
    }
    // Allocate memory using malloc
    void* ptr = customMalloc(total_size);
    if (ptr == NULL) {
        return NULL; // Allocation failed
    }

    // Initialize the allocated memory to zero
    memset(ptr, 0, total_size);

    return ptr;
}

void* customRealloc(void* ptr, size_t size){
    // Reallocate memory using your custom allocator
    // Return a pointer to the reallocated memory
    if(size <= 0) {
        printf("<realloc error>: passed nonpositive size\n");
        return NULL;
    }
    // If ptr is NULL, realloc should behave like malloc
    if (ptr == NULL) {
        return customMalloc(size);
    }
    
    Block* block = (Block*)ptr - 1; // get pointer to the block header

    if (block->size >= size)  // If the current block is large enough
    {
        // Should split blocks 
        if (block->size - size - ALIGN_TO_8BIT(sizeof(Block)) > sizeof(Block) + ALIGN_TO_8BIT(1))
        {
            // Split the block into two
            Block* new_block = (Block*)((char*)block + sizeof(Block) + size);
            new_block->size = block->size - size - sizeof(Block);
            new_block->free = true;
            // add block to linked list
            new_block->next = block->next;
            block->next = new_block;
        }
        block->size = size;
        return ptr;
    } else {
        // Allocate new memory using malloc
        void* new_ptr = customMalloc(size);
        if (new_ptr == NULL) {
            printf("<realloc error>: failed to allocate memory\n");
            return NULL;
        }
        
        // Copy data from old memory to new memory
        memcpy(new_ptr, ptr, block->size);
        
        // Free old memory
        customFree(ptr);

        return new_ptr;
    }

    return nullptr;
}


// return ptr to the beginning of the data, after header
Block* findFreeBlock(size_t size){
    // find best fit block
    Block* best_fit_block = nullptr;
    for (Block* curr_block = block_list; curr_block!= NULL; curr_block = curr_block->next){
        if (curr_block->free && curr_block->size >= size)
        {
            if(best_fit_block == nullptr)
                best_fit_block = curr_block;
            else if (best_fit_block->size > curr_block->size)
                best_fit_block = curr_block;
        }
    }

    if(best_fit_block != nullptr){
        //printf("found best_fit_block = size %lu\n", best_fit_block->size);
        return best_fit_block;
    }else{
        // No suitable block found, request more memory from the OS
        best_fit_block = requestSpace(size);
        if(best_fit_block == NULL){
            printf("Fatal ERROR!\n");
        }
        add_block_to_linked_list(best_fit_block);
        // return pointer to the beginning of the data block
        return best_fit_block + 1;
    }
}

Block* requestSpace(size_t size){
    // Request more memory from the operating system and create a new block
    // Return a pointer to the new block (start of header) or NULL if memory allocation fails
    Block* curr = (Block*)sbrk(0);
    void * result = sbrk(sizeof(Block)+ size);
    if (result == SBRK_FAIL) { // sbrk failed
        return NULL;
    }

    // Initialize the block
    curr->size = size;
    curr->next = NULL;
    curr->free = false;
    return curr;
}