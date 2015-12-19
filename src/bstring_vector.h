#ifndef _BSTRING_VECTOR_H_
#define _BSTRING_VECTOR_H_

#undef DATA_TYPE
#undef VECTOR_TYPE

#define DATA_TYPE bstring
#define VECTOR_TYPE BstringVector
#include "vectorgen.h"

#undef DATA_TYPE
#undef VECTOR_TYPE
#endif
