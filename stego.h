#ifndef HW_01_STEGO_H
#define HW_01_STEGO_H

#include "bmp.h"

void insertStegoData(Bitmap *bitmap, FILE* message, FILE *key);

void extractStegoData(const Bitmap *bitmap, FILE *key);

#endif //HW_01_STEGO_H
