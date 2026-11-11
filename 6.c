// Unions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef union {
    int i;
    float f;
    char str[20];
} UnionData;

typedef struct {
    int i;
    float f;
    char str[20];
} StructData;

int main() {

    UnionData udata;

    StructData sdata;
    // sdata = (StructData) {10, 220.5, "Struct Programming"};
    sdata.i = 10;
    strcpy(sdata.str, "Struct Programming");
    sdata.f = 220.5;

    udata.i = 10;
    printf("udata.i: %d\n", udata.i);

    strcpy(udata.str, "Union Programming");
    printf("udata.str: %s\n", udata.str);

    udata.f = 220.5;
    printf("udata.f: %.2f\n\n", udata.f);

    // Now see the values after overwriting
    printf("After overwriting:\n");
    printf("udata.i: %d\n", udata.i);
    printf("udata.str: %s\n", udata.str);
    printf("udata.f: %.2f\n\n", udata.f);

    // Memory Addresses - Union
    printf("udata.i: %p\n", &udata.i);
    printf("udata.str: %p\n", &udata.str);
    printf("udata.f: %p\n\n", &udata.f);

    // Memory Addresses - Struct
    printf("sdata.i: %p\n", &sdata.i);
    printf("sdata.str: %p\n", &sdata.str);
    printf("sdata.f: %p\n\n", &sdata.f);

    printf("Size of the Union - %lu\n", sizeof(UnionData));
    printf("Size of the Structure - %lu\n", sizeof(StructData));

    return 0;
}
