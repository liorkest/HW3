#include "customAllocator.h"
#include <stdio.h>


void print_break(const char* message) {
    void* program_break = sbrk(0);
    printf("%s: Current break is at %p\n", message, program_break);
}

int main() {
    print_break("Initial");
    //printf("size of block: %lu\n", sizeof(Block));
    // Allocate some memory blocks
    void* ptr1 = customMalloc(100);
    void* ptr2 = customMalloc(200);
    void* ptr3 = customMalloc(2007);
    void* ptr4 = customMalloc(17);
    print_break("After allocations");

    // Free the first block and check if merge happens (it won't reduce the break)
    customFree(ptr2);
    print_break("After free(ptr2)");

    // Free the second block and check if the break reduces
    customFree(ptr3);
    print_break("After free(ptr3)");

    customFree(ptr4);
    print_break("After free(ptr3)");

    // Allocate another block to see if it reuses the free space
    ptr3 = customMalloc(50);
    print_break("After reallocating ptr3");

    // Free the last block and check if the break reduces again
    customFree(ptr3);
    print_break("After free(ptr3)");

    customFree(ptr1);
    return 0;
}