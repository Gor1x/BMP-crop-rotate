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
    size_t widthBytes;
    BitmapData header;
    Pixel** picture;

} Bitmap;


static void scanHeader(Bitmap *bitmap, FILE *file);
static void scanSize(Bitmap *bitmap, FILE *file);
static void initPixelArray(Bitmap *bitmap);
static void scanPicture(Bitmap *bitmap, FILE *file);
static void reverse(Pixel **arr, size_t height, size_t width);

void readBitmap(Bitmap *bitmap, FILE *file);

static void copyPixelArray(const Bitmap *bitmap, Bitmap *dest, size_t x, size_t y, size_t width, size_t height);
static void initBitmapSize(Bitmap *bitmap, size_t width, size_t height);
static void initBitmapHeader(const Bitmap *bitmap, Bitmap *dest);

int crop(const Bitmap *bitmap, size_t x, size_t y, size_t width, size_t height, Bitmap *dest);


static void printHeader(Bitmap *bitmap, FILE *file);
static void printPicture(Bitmap *bitmap, FILE *file);

void saveBitmap(Bitmap *bitmap, FILE *file);


static void clearPicture(Bitmap *bitmap);

void clearBitmap(Bitmap *bitmap);

static void rotatePixels(const Bitmap *bitmap, Bitmap *dest);

void rotate(const Bitmap* bitmap, Bitmap *dest);

void printPixelArray(Bitmap *bitmap, FILE *file);

#endif //HW_01_BMP_H
