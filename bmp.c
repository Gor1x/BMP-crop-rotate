#include "bmp.h"


static size_t normalizeTo4(size_t x)
{
    return (x + 3) / 4 * 4;
}

static void scanHeader(Bitmap *bitmap, FILE *file)
{
    fseek(file, 0, SEEK_SET);
    fread(&bitmap->header, HEADER_SIZE, 1, file);
}

static void scanSize(Bitmap *bitmap, FILE *file)
{
    fseek(file, WIDTH_POSITION, SEEK_SET);

    bitmap->width = bitmap->height = 0;

    fread(&bitmap->width, WIDTH_BYTES_SIZE, 1, file);
    fread(&bitmap->height, HEIGHT_BYTES_SIZE, 1, file);

    bitmap->widthBytes = normalizeTo4(bitmap->width * PIXEL_SIZE);
}

static int initPixelArray(Bitmap *bitmap)
{
    bitmap->picture = (Pixel**)malloc(bitmap->height * sizeof(Pixel*));
    if (bitmap->picture == NULL)
        return 1;

    bitmap->picture[0] = (Pixel*)malloc(bitmap->widthBytes * bitmap->height);
    memset(bitmap->picture[0], 0, bitmap->widthBytes * bitmap->height);

    if (bitmap->picture[0] == NULL)
    {
        free(bitmap->picture);
        return 1;
    }

    for (size_t i = 1; i < bitmap->height; i++)
    {
        bitmap->picture[i] = (Pixel*)((char*)bitmap->picture[i - 1] + bitmap->widthBytes);
    }
    return 0;
}

static void reverse(Pixel **arr, size_t height, size_t width)
{
    for (size_t i = 0; i * 2 < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            Pixel tmp = arr[i][j];
            arr[i][j] = arr[height - i - 1][j];
            arr[height - i - 1][j] = tmp;
        }
    }
}


static void scanPicture(Bitmap *bitmap, FILE *file)
{
    fseek(file, sizeof(bitmap->header), SEEK_SET);
    fread(&bitmap->picture[0][0], 1, bitmap->widthBytes * bitmap->height, file);
    reverse(bitmap->picture, bitmap->height, bitmap->width);
}

int readBitmap(Bitmap *bitmap, FILE *file)
{
    scanHeader(bitmap, file);
    scanSize(bitmap, file);
    if (initPixelArray(bitmap) != 0)
    {
        return 1;
    }
    scanPicture(bitmap, file);
    return 0;
}

static void copyPixelArray(const Bitmap *bitmap, Bitmap *dest, size_t x, size_t y, size_t width, size_t height)
{
    for (size_t i = y; i < y + height; i++)
    {
        for (size_t j = x; j < x + width; j++)
        {
            dest->picture[i - y][j - x] = bitmap->picture[i][j];
        }
    }
}

static void initBitmapSize(Bitmap *bitmap, size_t width, size_t height)
{
    bitmap->height = height;
    bitmap->width = width;
    bitmap->widthBytes = normalizeTo4(bitmap->width * PIXEL_SIZE);
}

static void initBitmapHeader(const Bitmap *bitmap, Bitmap *dest)
{
    dest->header = bitmap->header;

    size_t pixelSize = dest->widthBytes * dest->height;

    memcpy(&dest->header.biSizeImage, &pixelSize, sizeof(dest->header.biSizeImage));

    size_t fileSize = pixelSize + sizeof(dest->header);
    memcpy(&dest->header.bfSizeFile, &fileSize, sizeof(dest->header.bfSizeFile));

    memcpy(&dest->header.biWidth, &dest->width, sizeof(dest->header.biWidth));
    memcpy(&dest->header.biHeight, &dest->height, sizeof(dest->header.biHeight));
}

int crop(const Bitmap *bitmap, size_t x, size_t y, size_t width, size_t height, Bitmap *dest)
{
    initBitmapSize(dest, width, height);
    initBitmapHeader(bitmap, dest);
    if (initPixelArray(dest) != 0)
    {
        return 1;
    }
    copyPixelArray(bitmap, dest, x, y, width, height);
    return 0;
}

static void printHeader(const Bitmap *bitmap, FILE *file)
{
    fwrite(&bitmap->header, HEADER_SIZE, 1, file);
}

static void printPicture(const Bitmap *bitmap, FILE *file)
{
    fseek(file, sizeof(bitmap->header), SEEK_SET);

    reverse(bitmap->picture, bitmap->height, bitmap->width);

    fwrite(&bitmap->picture[0][0], bitmap->height * bitmap->widthBytes, 1, file);

    reverse(bitmap->picture, bitmap->height, bitmap->width);
}

void saveBitmap(const Bitmap *bitmap, FILE *file)
{
    printHeader(bitmap, file);
    printPicture(bitmap, file);
}

void clearBitmap(Bitmap *bitmap)
{
    free(bitmap->picture[0]);
    free(bitmap->picture);
}

static void rotatePixels(const Bitmap *bitmap, Bitmap *dest)
{
    for (size_t i = 0; i < bitmap->height; i++)
    {
        for (size_t j = 0; j < bitmap->width; j++)
        {
            dest->picture[j][bitmap->height - i - 1] = bitmap->picture[i][j];
        }
    }
}

int rotate(const Bitmap* bitmap, Bitmap *dest)
{
    initBitmapSize(dest, bitmap->height, bitmap->width);
    initBitmapHeader(bitmap, dest);

    if (initPixelArray(dest) != 0)
    {
        return 1;
    }

    rotatePixels(bitmap, dest);
    return 0;
}
