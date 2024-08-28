#include "customAllocator.h"
#include <stdio.h>
#include <assert.h>

void print_break(const char* message) {
    void* program_break = sbrk(0);
    printf("%s: Current break is at %p\n", message, program_break);
}

void test_customMalloc() {
    printf("\nRunning test_customMalloc...\n");
    
    void* ptr1 = customMalloc(32);
    assert(ptr1 != NULL);
    printf("Allocated 32 bytes at %p\n", ptr1);

    void* ptr2 = customMalloc(64);
    assert(ptr2 != NULL);
    printf("Allocated 64 bytes at %p\n", ptr2);

    print_list();
}

void test_customFree() {
    printf("\nRunning test_customFree...\n");

    void* ptr1 = customMalloc(32);
    void* ptr2 = customMalloc(64);

    customFree(ptr1);
    printf("Freed memory at %p\n", ptr1);

    customFree(ptr2);
    printf("Freed memory at %p\n", ptr2);

    print_list();
}

void test_merge_free_blocks() {
    printf("\nRunning test_merge_free_blocks...\n");

    void* ptr1 = customMalloc(32);
    void* ptr2 = customMalloc(64);

    customFree(ptr1);
    customFree(ptr2);

    printf("Merged adjacent free blocks\n");

    print_list();
}

void test_customCalloc() {
    printf("\nRunning test_customCalloc...\n");

    void* ptr = customCalloc(4, 16);
    assert(ptr != NULL);
    printf("Allocated and zeroed 4 elements of 16 bytes each at %p\n", ptr);

    // Verify that the allocated memory is indeed zeroed
    unsigned char* data = (unsigned char*)ptr;
    if(data == NULL) return;
    for (size_t i = 0; i < 64; i++) {
        assert(data[i] == 0);
    }
    print_list();
}

void test_customRealloc() {
    printf("\nRunning test_customRealloc...\n");
    print_list();
    void* ptr1 = customMalloc(32);
    print_list();
    void* ptr2 = customRealloc(ptr1, 64);
    assert(ptr2 != NULL);
    printf("Reallocated memory from 32 bytes to 64 bytes at %p\n", ptr2);

    print_list();
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


    test_customMalloc();
    test_customFree();
    test_merge_free_blocks();
    test_customCalloc();
    test_customRealloc();

    printf("\nAll tests passed!\n");
    return 0;
}

