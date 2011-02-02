#ifndef EC_H
#define EC_H

#include "util.h"

#include <stddef.h> // size_t

int generateErrorCorrectionCode(const byte* const data, size_t dataSize, byte* dest, size_t ecBlocks);

void calculate_galois_field(const int pp);

byte altope(const int alpha);
int petoal(const byte integer);

#endif //EC_H

