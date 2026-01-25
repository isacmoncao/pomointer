#ifndef POMOINTER_PREPROCESSOR_H
#define POMOINTER_PREPROCESSOR_H

#include <stdio.h>

#define MAX_INCLUDE_DEPTH 10

FILE* preprocess_file(const char* path, int depth);

#endif
