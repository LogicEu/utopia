#include "bucket.h"
#include USTDLIB_H
#include USTRING_H

size_t bucket_size(const bucket_t bucket)
{
    return bucket ? bucket[BUCKET_SIZE_INDEX] : 0;
}

size_t bucket_capacity(const bucket_t bucket)
{
    return bucket ? bucket[BUCKET_CAP_INDEX] : 0;
}

index_t* bucket_data(const bucket_t bucket)
{
    return bucket ? bucket + BUCKET_DATA_INDEX : NULL;
}

void bucket_reindex(bucket_t bucket, const index_t removed)
{
    size_t i;
    const size_t count = bucket_size(bucket) + BUCKET_DATA_INDEX;
    for (i = BUCKET_DATA_INDEX; i < count; i++) {
        bucket[i] -= (bucket[i] >= removed);
    }
}

bucket_t bucket_push(bucket_t bucket, const index_t index)
{
    const size_t size = bucket_size(bucket);
    const size_t cap = bucket_capacity(bucket);

    if (size == cap) {
        const size_t capacity = cap * 2 + !cap;
        bucket = realloc(bucket, (capacity + BUCKET_DATA_INDEX) * sizeof(index_t));
        bucket[BUCKET_CAP_INDEX] = capacity;
    }

    bucket[BUCKET_DATA_INDEX + size] = index;
    bucket[BUCKET_SIZE_INDEX] = size + 1;

    return bucket;
}

void bucket_remove(bucket_t bucket, const size_t index)
{
    const size_t size = bucket_size(bucket) + BUCKET_DATA_INDEX;
    index_t* ptr = bucket + index;
    memmove(ptr, ptr + 1, (size - index - 1) * sizeof(index_t));
    bucket[BUCKET_SIZE_INDEX] = size - 1 - BUCKET_DATA_INDEX;
}

void buckets_reindex(bucket_t*buckets, const size_t size, const index_t removed)
{
    size_t i;
    for (i = 0; i < size; ++i) {
        if (buckets[i]) {
            bucket_reindex(buckets[i], removed);
        }
    }
}

void buckets_free(bucket_t*buckets, const size_t size)
{
    size_t i;
    for (i = 0; i < size; ++i) {
        if (buckets[i]) {
            free(buckets[i]);
        }
    }
}
