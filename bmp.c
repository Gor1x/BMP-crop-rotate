#include "bmp.h"

static void scanHeader(Bitmap *bitmap, FILE *file)
{
    fseek(file, 0, SEEK_SET);
    BitmapData *header = &bitmap->header;

    //HEADER
    fread(header->bfType, sizeof(header->bfType), 1, file);
    fread(header->bfSizeFile, sizeof(header->bfSizeFile), 1, file);
    fread(header->bfHeaderOtherFirst, sizeof(header->bfHeaderOtherFirst), 1, file);

    //INFO
    fread(header->biWidth, sizeof(header->biWidth), 1, file);
    fread(header->biHeight, sizeof(header->biHeight), 1, file);
    fread(header->biOtherFirst, sizeof(header->biOtherFirst), 1, file);
    fread(header->biSizeImage, sizeof(header->biSizeImage), 1, file);
    fread(header->biOtherSecond, sizeof(header->biOtherSecond), 1, file);
}

static void scanSize(Bitmap *bitmap, FILE *file)
{
    fseek(file, 18, SEEK_SET);

    fread(&bitmap->height, 4, 1, file);
    fread(&bitmap->width, 4, 1, file);

    bitmap->fileWidth = (bitmap->width + 3) / 4 * 4;
}

static void initPixelArray(Bitmap *bitmap)
{
    bitmap->picture = (Pixel**)malloc(bitmap->height * sizeof(Pixel*));
    bitmap->picture[0] = (Pixel*)malloc(bitmap->height * bitmap->fileWidth * sizeof(Pixel));

    for (size_t i = 1; i < bitmap->height; i++)
    {
        bitmap->picture[i] = bitmap->picture[i - 1] + bitmap->fileWidth;
    }
}

static void swap(void **a, void **b)
{
    void *tmp = *a;
    *a = *b;
    *b = tmp;
}

static void reverse(void **arr, size_t length)
{
    for (int i = 0; i * 2 < length; i++)
    {
        swap(&arr[i], &arr[length - i - 1]);
    }
}

static void scanPicture(Bitmap *bitmap, FILE *file)
{
    fseek(file, 54, SEEK_SET);
    fread(bitmap->picture[0], PIXEL_SIZE, bitmap->height * bitmap->fileWidth, file);
    reverse((void **)bitmap->picture, bitmap->height);
}

void readBitmap(Bitmap *bitmap, FILE *file)
{
    scanHeader(bitmap, file);
    scanSize(bitmap, file);
    initPixelArray(bitmap);
    scanPicture(bitmap, file);
}

static void copyPixelArray(Bitmap *bitmap, Bitmap *dest, size_t x, size_t y, size_t width, size_t height)
{
    for (size_t i = x; i < x + width; i++)
    {
        for (size_t j = y; j < y + height; j++)
        {
            dest->picture[i - x][j - y] = bitmap->picture[i][j];
        }
    }
}

static void initBitmapSize(Bitmap *bitmap, size_t width, size_t height)
{
    bitmap->height = height;
    bitmap->width = width;
    bitmap->fileWidth = (width + 3) / 4 * 4;
}

static void initBitmapHeader(Bitmap *bitmap, Bitmap *dest)
{
    dest->header = bitmap->header;

    size_t pixelSize = dest->fileWidth * dest->height;
    memcpy(&dest->header.biSizeImage, &pixelSize, sizeof(dest->header.biSizeImage));

    size_t fileSize = pixelSize + 54;
    memcpy(&dest->header.bfSizeFile, &fileSize, sizeof(dest->header.bfSizeFile));

    memcpy(&dest->height, &dest->height, sizeof(dest->height));
    memcpy(&dest->width, &dest->width, sizeof(dest->width));
}

int crop(Bitmap *bitmap, size_t x, size_t y, size_t width, size_t height, Bitmap *dest)
{
    initBitmapHeader(bitmap, dest);
    initBitmapSize(bitmap, width, height);
    initPixelArray(dest);
    copyPixelArray(bitmap, dest, x, y, width, height);
    return 0;
}


void saveBitmap(Bitmap *bitmap, FILE *file)
{

}