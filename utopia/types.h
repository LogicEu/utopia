#ifndef UTOPIA_TYPES_H
#define UTOPIA_TYPES_H

#include <utopia/config.h>

#ifndef USTDDEF_H
#define USTDDEF_H <stddef.h>
#endif

#include USTDDEF_H

#ifndef UTOPIA_INDEX_TYPE
#define UTOPIA_INDEX_TYPE size_t
#endif

typedef UTOPIA_INDEX_TYPE index_t;
typedef index_t* bucket_t;

#endif /* UTOPIA_TYPES_H */

