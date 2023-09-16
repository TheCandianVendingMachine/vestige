#include <stdio.h>

#include <lib/hashmap.h>

uint64_t hash_int(const void* p) {
    const int* i = (const int*)p;
    return gethash(i, sizeof(int));
}

int main(int argc, char* argv[]) {
    HashMap m = HASHMAP(int);

    int i = 1;
    hashmap_set(&m, "1", &i);
    i = 3;
    hashmap_set(&m, "3", &i);

    printf("value of '1': %d\n", *(const int*)hashmap_get(&m, "1"));
    printf("value of '3': %d\n", *(const int*)hashmap_get(&m, "3"));
    printf("value of '4': %p\n", hashmap_get(&m, "4"));

    return 0;
}
