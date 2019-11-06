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
    bitmap->width = bitmap->height = 0;

    fread(&bitmap->width, 4, 1, file);
    fread(&bitmap->height, 4, 1, file);

    bitmap->widthBytes = (bitmap->width * 3 + 3) / 4 * 4;
}

static void initPixelArray(Bitmap *bitmap)
{
    bitmap->picture = (Pixel**)malloc(bitmap->height * sizeof(Pixel*));
    for (size_t i = 0; i < bitmap->height; i++)
    {
        bitmap->picture[i] = (Pixel*)calloc(bitmap->widthBytes, sizeof(Pixel));
    }
}

static void printPixel(Pixel a, FILE *file)
{
    for (int i = 0; i < 3; i++)
        fprintf(file, "%d ", (int)a.data[i]);
    fprintf(file, " ");
}

void printPixelArray(Bitmap *bitmap, FILE *file)
{
    size_t height = bitmap->height;
    size_t width = bitmap->width;

    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            printPixel(bitmap->picture[i][j], file);
            fprintf(file, " | ");
        }
        fprintf(file, "\n");
    }
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
    fseek(file, 54, SEEK_SET);
    for (size_t i = 0; i < bitmap->height; i++)
    {
        for (size_t j = 0; j < bitmap->width; j++)
        {
            fread(&bitmap->picture[i][j], PIXEL_SIZE, 1, file);
        }
        fseek(file, bitmap->widthBytes - bitmap->width * 3, SEEK_CUR);
    }
    reverse(bitmap->picture, bitmap->height, bitmap->width);
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
    bitmap->widthBytes = (width * 3 + 3) / 4 * 4;
}

static void initBitmapHeader(Bitmap *bitmap, Bitmap *dest)
{
    dest->header = bitmap->header;

    size_t pixelSize = dest->widthBytes * dest->height;

    memcpy(&dest->header.biSizeImage, &pixelSize, sizeof(dest->header.biSizeImage));

    size_t fileSize = pixelSize + 54;
    memcpy(&dest->header.bfSizeFile, &fileSize, sizeof(dest->header.bfSizeFile));

    memcpy(&dest->header.biWidth, &dest->width, sizeof(dest->header.biWidth));
    memcpy(&dest->header.biHeight, &dest->height, sizeof(dest->header.biHeight));
}

int crop(Bitmap *bitmap, size_t x, size_t y, size_t width, size_t height, Bitmap *dest)
{
    initBitmapSize(dest, width, height);
    initBitmapHeader(bitmap, dest);
    initPixelArray(dest);
    copyPixelArray(bitmap, dest, x, y, width, height);
    return 0;
}

static void printHeader(Bitmap *bitmap, FILE *file)
{
    BitmapData *header = &bitmap->header;

    fwrite(&header->bfType, sizeof(header->bfType), 1, file);
    fwrite(&header->bfSizeFile, sizeof(header->bfSizeFile), 1, file);
    fwrite(&header->bfHeaderOtherFirst, sizeof(header->bfHeaderOtherFirst), 1, file);

    fwrite(&header->biWidth, sizeof(header->biWidth), 1, file);
    fwrite(&header->biHeight, sizeof(header->biHeight), 1, file);
    fwrite(&header->biOtherFirst, sizeof(header->biOtherFirst), 1, file);
    fwrite(&header->biSizeImage, sizeof(header->biSizeImage), 1, file);
    fwrite(&header->biOtherSecond, sizeof(header->biOtherSecond), 1, file);
}

static void printZeros(size_t count, FILE *file)
{
    for (size_t i = 0; i < count; i++)
    {
        int a = 0;
        fwrite(&a, 1, 1, file);
    }
}

static void printPicture(Bitmap *bitmap, FILE *file)
{
    fseek(file, 54, SEEK_SET);
    reverse(bitmap->picture, bitmap->height, bitmap->width);
    for (size_t i = 0; i < bitmap->height; i++)
    {
        for (size_t j = 0; j < bitmap->width; j++)
        {
            fwrite(&bitmap->picture[i][j].data, PIXEL_SIZE, 1, file);
        }
        printZeros(bitmap->widthBytes - (bitmap->width * PIXEL_SIZE), file);
    }
    reverse(bitmap->picture, bitmap->height, bitmap->width);
}

void saveBitmap(Bitmap *bitmap, FILE *file)
{
    printHeader(bitmap, file);
    printPicture(bitmap, file);
}

static void clearPicture(Bitmap *bitmap)
{
    free(bitmap->picture[0]);
    free(bitmap->picture);
}

void clearBitmap(Bitmap *bitmap)
{
    clearPicture(bitmap);
}

static void rotatePixels(const Bitmap *bitmap, Bitmap *dest)
{
    for (size_t i = 0; i < bitmap->height; i++)
    {
        for (size_t j = 0; j < bitmap->width; j++)
        {
            dest->picture[bitmap->width - j - 1][i] = bitmap->picture[i][j];
        }
    }
}

void rotate(const Bitmap* bitmap, Bitmap *dest)
{
    initBitmapSize(dest, bitmap->height, bitmap->width);
    initBitmapHeader(bitmap, dest);
    initPixelArray(dest);
    rotatePixels(bitmap, dest);
}