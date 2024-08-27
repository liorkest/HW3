#include "customAllocator.h"
#include <stdio.h>

extern Block* blockList;

void add_block_to_linked_list(Block* new_block){
    if(block_list == nullptr) 
        block_list = new_block;
    Block* block;
    for(block = block_list; block != NULL; block = block->next);
    // Add the new block to the end of the linked list
    block->next = new_block;
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
    // Free the allocated memory using your custom allocator
    // Example: free(ptr);
    if (!ptr) {
        printf("<free error>: passed null pointer\n");
        return;
    }

    Block* block = (Block*)ptr - 1;
    block->free = 1;
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
    // Find a free block in the memory pool that can accommodate the given size
    // Return a pointer to the found block or NULL if no suitable block is found

    // find best fit block
    Block* best_fit_block = nullptr;
    for (Block* curr_block = blockList; curr_block!= NULL; curr_block = curr_block->next){
        if (curr_block->free && curr_block->size >= size)
        {
            if(best_fit_block == nullptr)
                best_fit_block = curr_block;
            else if (best_fit_block->size > curr_block->size)
                best_fit_block = curr_block;
        }
    }

    if(best_fit_block != nullptr){
        return best_fit_block;
    }else{
        // No suitable block found, request more memory from the OS
        best_fit_block = requestSpace(size);
        add_block_to_linked_list(best_fit_block);
        if(best_fit_block == nullptr){
            printf("Fatal ERROR!\n");
        }
        return best_fit_block + 1;
    }
}

Block* requestSpace(size_t size){
    // Request more memory from the operating system and create a new block
    // Return a pointer to the new block or NULL if memory allocation fails
    Block* curr = (Block*)sbrk(0);
    void * result = sbrk(size + sizeof(Block));
    if (result == SBRK_FAIL) { // sbrk failed
        return NULL;
    }

    // Initialize the block
    curr->size = size;
    curr->next = NULL;
    curr->free = 0;
    return curr;
}