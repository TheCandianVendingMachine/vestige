#include <stdio.h>

#include <lib/hashmap.h>

#include "test_info.h"

// uint64_t hash_int(const void* p) {
//     const int* i = (const int*)p;
//     return gethash(i, sizeof(int));
// }

void test_hashmap(void* complete) {
    TestInfo* info = (TestInfo*)complete;

    HashMap m = GHASHMAP(int, cstrhash);
    char buf[32];
    for (int i = 0; i < 32; i++) {
        sprintf(buf, "%d", i);
        hashmap_set(&m, buf, &i);
    }

    fprintf(info->test_stream, "%d\n", *(const int*)hashmap_get(&m, "1"));
    fprintf(info->test_stream, "%d\n", *(const int*)hashmap_get(&m, "31"));
    fprintf(info->test_stream, "%d\n", *(const int*)hashmap_delete(&m, "1"));
    fprintf(info->test_stream, "%p\n", hashmap_get(&m, "1"));

    info->status = SUCCESS;
}
