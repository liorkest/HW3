#include "customAllocator.h"
#include <stdio.h>
int main(){
    // Allocate memory using the custom allocator
    int* p = (int*)customMalloc(sizeof(int) * 10);
    for(int i = 0; i <10;i++){
        p[i] = i*10;
        printf("The value saved in p[%d] is %d",i,p[i]);
    }
    // Do nasty stuff:
    //p[len] = 5;
}