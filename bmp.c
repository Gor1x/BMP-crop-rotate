#include "bmp.h"

const size_t WIDTH_POSITION = 18;
const size_t WIDTH_BYTES_SIZE = 4;
const size_t HEIGHT_BYTES_SIZE = 4;

static size_t normalizeTo4(size_t x)
{
    return (x + 3) / 4 * 4;
}

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

    bitmap->picture[0] = (Pixel*)calloc(bitmap->width * bitmap->height, sizeof(Pixel));
    if (bitmap->picture[0] == NULL)
    {
        free(bitmap->picture);
        return 1;
    }
    for (size_t i = 1; i < bitmap->height; i++)
    {
        bitmap->picture[i] = bitmap->picture[i - 1] + bitmap->width;
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

    for (size_t i = 0; i < bitmap->height; i++)
    {
        fread(&bitmap->picture[i][0], PIXEL_SIZE, bitmap->width, file);
        fseek(file, (int)bitmap->widthBytes - (int)bitmap->width * PIXEL_SIZE, SEEK_CUR);
    }

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
    const BitmapData *header = &bitmap->header;

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

static void printPicture(const Bitmap *bitmap, FILE *file)
{
    fseek(file, sizeof(bitmap->header), SEEK_SET);
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
            dest->picture[bitmap->width - j - 1][i] = bitmap->picture[i][j];
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
