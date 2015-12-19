#include "bstrlib.h"

#undef DATA_TYPE
#undef VECTOR_TYPE

#define DATA_TYPE float
#define VECTOR_TYPE FloatVector
#include "vectorgen.c"

#undef DATA_TYPE
#undef VECTOR_TYPE
