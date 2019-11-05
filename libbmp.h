//
// Created by egor on 05.11.2019.
//

#ifndef HW_01_LIBBMP_H
#define HW_01_LIBBMP_H

#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"

#define HEADER_SIZE 52
#define PIXEL_SIZE 3


typedef struct Pixel_s
{
    char data[PIXEL_SIZE];
} Pixel;

typedef struct Bitmap_s
{
    size_t width;
    size_t height;
    size_t fileWidth;
    char header[HEADER_SIZE];
    Pixel** picture;
} Bitmap;


static void scanHeader(Bitmap *bitmap, FILE *file);
static void scanSize(Bitmap *bitmap, FILE *file);
static void initPixelArray(Bitmap *bitmap);
static void scanPicture(Bitmap *bitmap, FILE *file);

void readBitmap(Bitmap *bitmap, FILE *file);

#endif //HW_01_LIBBMP_H
