/*
 * pomointer - .pf file interpreter
 * Copyright (c) 2026 José Isac Araujo Monção
 * 
 * See LICENSE file for full BSD 3-Clause license terms.
 */
#ifndef POMOINTER_PREPROCESSOR_H
#define POMOINTER_PREPROCESSOR_H

#include <stdio.h>

#define MAX_INCLUDE_DEPTH 10

FILE* preprocess_file(const char* path, int depth);

#endif
