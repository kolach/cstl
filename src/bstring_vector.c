#include "bstrlib.h"

#undef DATA_TYPE
#undef VECTOR_TYPE

#define DATA_TYPE bstring
#define VECTOR_TYPE BstringVector
#include "vectorgen.c"

#undef DATA_TYPE
#undef VECTOR_TYPE
