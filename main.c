#include "libbmp.h"
#include "stdio.h"

int main()
{
    Bitmap bitmap;
    FILE *file = fopen("../lena_512.bmp", "rb");
    readBitmap(&bitmap, file);

    return  0;
}