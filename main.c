#include "bmp.h"
#include "stdio.h"

int main()
{
    Bitmap bitmap;

    debug("File opening");
    FILE *file = fopen("../lena_512.bmp", "rb");
    if (file == NULL)
    {
        debug("Failed to open file");
        return -1;
    }
    fclose(file);


    readBitmap(&bitmap, file);
    debug("Bitmap is read");

    Bitmap result;
    crop(&bitmap, 100, 100, 256, 256, &result);
    clearBitmap(&bitmap);

    debug("Cropped");

    Bitmap rotated;
    rotate(&result, &rotated);
    debug("Rotated");

    FILE* to = fopen("../lena_228.bmp", "wb");
    saveBitmap(&rotated, to);
    clearBitmap(&rotated);
    clearBitmap(&result);
    fclose(to);
    return  0;
}