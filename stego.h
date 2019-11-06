#ifndef HW_01_STEGO_H
#define HW_01_STEGO_H

#include "bmp.h"
#include "stdbool.h"

int insertStegoData(Bitmap *bitmap, FILE* message, FILE *key);

void extractStegoData(const Bitmap *bitmap, FILE *key, FILE *message);

#endif //HW_01_STEGO_H
