#include "bmp.h"
#include "stdio.h"

int main()
{
    Bitmap bitmap;
    FILE *file = fopen("../lena_512.bmp", "rb");
    readBitmap(&bitmap, file);

    Bitmap result;
    crop(&bitmap, 0, 0, 512, 512, &result);

    FILE* to = fopen("../lena_228.bmp", "wb");
    saveBitmap(&result, to);

    return  0;
}