#include "libbmp.h"

static void scanHeader(Bitmap *bitmap, FILE *file)
{
    fseek(file, 0, SEEK_SET);
    fread(&bitmap->header, HEADER_SIZE, 1, file);
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

static void scanPicture(Bitmap *bitmap, FILE *file)
{
    fseek(file, 54, SEEK_SET);
    fread(bitmap->picture[0], PIXEL_SIZE, bitmap->height * bitmap->fileWidth, file);
}

void readBitmap(Bitmap *bitmap, FILE *file)
{
    scanHeader(bitmap, file);
    scanSize(bitmap, file);
    scanPicture(bitmap, file);
}

Bitmap cropBitmap(Bitmap *bitmap, size_t x, size_t y, size_t width, size_t height)
{

}