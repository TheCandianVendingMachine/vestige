#include <stdio.h>

#include <lib/hashmap.h>

// uint64_t hash_int(const void* p) {
//     const int* i = (const int*)p;
//     return gethash(i, sizeof(int));
// }

int main(int argc, char* argv[]) {
    HashMap m = GHASHMAP(int, cstrhash);
    char buf[32];
    for (int i = 0; i < 32; i++) {
	sprintf(buf, "%d", i);
	hashmap_set(&m, buf, &i);
    }

    printf("%d\n", *(const int*)hashmap_get(&m, "1"));
    printf("%d\n", *(const int*)hashmap_get(&m, "31"));
    printf("%d\n", *(const int*)hashmap_delete(&m, "1"));
    printf("%p\n", hashmap_get(&m, "1"));

    return 0;
}
