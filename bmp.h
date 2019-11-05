#ifndef HW_01_BMP_H
#define HW_01_BMP_H

#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define PIXEL_SIZE 3

#define debug(x) printf("DEBUG: "); printf(x); printf("\n"); fflush(stdout);

typedef struct Pixel_s
{
    char data[PIXEL_SIZE];
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
} BitmapData;

typedef struct Bitmap_s
{
    size_t width;
    size_t height;
    size_t fileWidth;
    BitmapData header;
    Pixel** picture;

} Bitmap;


static void scanHeader(Bitmap *bitmap, FILE *file);
static void scanSize(Bitmap *bitmap, FILE *file);
static void initPixelArray(Bitmap *bitmap);
static void scanPicture(Bitmap *bitmap, FILE *file);
static void swap(void **a, void **b);
static void reverse(void **arr, size_t length);

void readBitmap(Bitmap *bitmap, FILE *file);


static void copyPixelArray(Bitmap *bitmap, Bitmap *dest, size_t x, size_t y, size_t width, size_t height);
static void initBitmapSize(Bitmap *bitmap, size_t width, size_t height);
static void initBitmapHeader(Bitmap *bitmap, Bitmap *dest);

int crop(Bitmap *bitmap, size_t x, size_t y, size_t width, size_t height, Bitmap *dest);


static void printHeader(Bitmap *bitmap, FILE *file);
static void printPicture(Bitmap *bitmap, FILE *file);

void saveBitmap(Bitmap *bitmap, FILE *file);


static void clearPicture(Bitmap *bitmap);

void clearBitmap(Bitmap *bitmap);

#endif //HW_01_BMP_H