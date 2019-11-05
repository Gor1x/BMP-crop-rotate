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

    readBitmap(&bitmap, file);
    debug("Bitmap is read");


    Bitmap result;
    debug("Cropping");

    crop(&bitmap, 0, 0, 512, 512, &result);

    debug("Cropped");
    FILE* to = fopen("../lena_228.bmp", "wb");
    saveBitmap(&result, to);


    clearBitmap(&bitmap);
    clearBitmap(&result);
    fclose(file);
    fclose(to);
    return  0;
}