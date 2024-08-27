#ifndef __CUSTOM_ALLOCATOR__
#define __CUSTOM_ALLOCATOR__

/*=============================================================================
* do no edit lines below!
=============================================================================*/
#include <stddef.h> //for size_t
#include <unistd.h>

/*=============================================================================
* Block
=============================================================================*/
typedef struct Block
{
    size_t size;
    struct Block* next;
    bool free;
} Block;


void* customMalloc(size_t size);
void customFree(void* ptr);
void* customCalloc(size_t nmemb, size_t size);
void* customRealloc(void* ptr, size_t size);
/*=============================================================================
* do no edit lines above!
=============================================================================*/

//below are suggestions - feel free to remove/change them
/*=============================================================================
* defines
=============================================================================*/
#define SBRK_FAIL (void*)(-1)
#define ALIGN_TO_8BIT(x) (((((x) - 1) >> 2) << 2) + 4)

Block* findFreeBlock(size_t size);

Block* requestSpace(size_t size);


#endif // CUSTOM_ALLOCATOR
