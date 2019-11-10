#ifndef HW_01_BMP_H
#define HW_01_BMP_H

#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

static const size_t PIXEL_SIZE = 3;
static const size_t WIDTH_POSITION = 18;
static const size_t WIDTH_BYTES_SIZE = 4;
static const size_t HEIGHT_BYTES_SIZE = 4;
static const size_t HEADER_SIZE = 54;

typedef struct Pixel_s
{
    unsigned char data[3];
} Pixel;

typedef struct BitmapData_s
{
    //HEADER
    char bfType[2];
    char bfSizeFile[4]; //Changing

    //BOTH
    char bfHeaderOtherFirst[12];

    //INFO
    char biWidth[4];
    char biHeight[4];
    char biOtherFirst[8]; //biPlanes, biBitCount, biCompression
    char biSizeImage[4]; //Changing
    char biOtherSecond[16]; //biXPelsPerMeters, biYPelsPerMeter, biClrUsed, biClrImportant
}  __attribute__((packed)) BitmapData;

typedef struct Bitmap_s
{
    size_t width;
    size_t height;
    size_t widthBytes;
    BitmapData header;
    Pixel** picture;

} Bitmap;


int readBitmap(Bitmap *bitmap, FILE *file);

int crop(const Bitmap *bitmap, size_t x, size_t y, size_t width, size_t height, Bitmap *dest);

void saveBitmap(const Bitmap *bitmap, FILE *file);

void clearBitmap(Bitmap *bitmap);

int rotate(const Bitmap* bitmap, Bitmap *dest);

#endif //HW_01_BMP_H
