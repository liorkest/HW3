#include "customAllocator.h"
#include <stdio.h>

Block* block_list = nullptr;

void merge_free_blocks(){
    // Merge adjacent free blocks
    Block* block;
    for(block = block_list; block != nullptr && block->next!= nullptr; block = block->next){
        if(block->free && block->next->free){
            block->size = block->size + sizeof(Block) + block->next->size; // size of merged block
            block->next = block->next->next;
        }
    }
}

void rm_block_from_linked_list(Block* b){
    if(b == block_list) {
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

    // find the beginning of the segment that is free at the end of heap
    Block* free_block = nullptr, *curr = block_list;
    for(; curr != nullptr; curr = curr->next){
        // save first free block after non-free
        if (curr->free && free_block == nullptr)  
            free_block = curr;
        // delete if there is no sequence of free blocks
        if(curr->free != 1)    
            free_block=nullptr;
    }

    // Check if the last block is free
    if (free_block->free) {
        // Check if the current break can be reduced
        void* program_break = sbrk(0); // Get current program break
        void* block_end = (void*)((char*)free_block + sizeof(Block) + free_block->size);

        if (block_end == program_break) {
            // Reduce the program break to release the memory back to the OS
            if (sbrk((sizeof(Block) + free_block->size)) == SBRK_FAIL) {
                // sbrk failed; could not reduce the break
                return;
            }
            rm_block_from_linked_list(free_block);
        }
    }
}

void add_block_to_linked_list(Block* new_block){
    if(block_list == nullptr) {
        printf("Initialized block list\n");
        block_list = new_block;
    } else {
        // Add the new block to the end of the linked list
        Block* block;
        for(block = block_list; block != NULL; block = block->next);
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
    if (block->free) {
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
    // Example: return calloc(nmemb, size);
    return nullptr;
}

void* customRealloc(void* ptr, size_t size){
    // Reallocate memory using your custom allocator
    // Return a pointer to the reallocated memory
    // Example: return realloc(ptr, size);
    return nullptr;
}

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
        printf("found best_fit_block = size %lu\n", best_fit_block->size);
        return best_fit_block;
    }else{
        // No suitable block found, request more memory from the OS
        best_fit_block = requestSpace(size);
        add_block_to_linked_list(best_fit_block);
        if(best_fit_block == nullptr){
            printf("Fatal ERROR!\n");
        }
        // return pointer to the beginning of the data block
        return best_fit_block + 1;
    }
}

Block* requestSpace(size_t size){
    // Request more memory from the operating system and create a new block
    // Return a pointer to the new block or NULL if memory allocation fails
    Block* curr = (Block*)sbrk(0);
    void * result = sbrk(sizeof(Block)+ size);
    if (result == SBRK_FAIL) { // sbrk failed
        return NULL;
    }

    // Initialize the block
    curr->size = size;
    curr->next = NULL;
    curr->free = 0;
    return curr;
}