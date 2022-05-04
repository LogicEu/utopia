#ifndef UTOPIA_BUCKET_H
#define UTOPIA_BUCKET_H

#ifdef _cplusplus
extern "C" {
#endif

#include <utopia.h>

#define BUCKET_CAP_INDEX 0
#define BUCKET_SIZE_INDEX 1
#define BUCKET_DATA_INDEX 2

bucket_t bucket_push(bucket_t bucket, const index_t index);
void bucket_remove(bucket_t bucket, const size_t index);
void bucket_reindex(bucket_t bucket, const index_t removed);

void buckets_free(bucket_t* buckets, const size_t size);
void buckets_reindex(bucket_t* buckets, const size_t size, const index_t removed);

#ifdef _cplusplus
}
#endif
#endif