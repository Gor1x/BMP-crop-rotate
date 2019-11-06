#include "stdbool.h"
#include "bmp.h"
#include "stdio.h"

static void error(const char *type)
{
    printf("Error: %s\n", type);
}

static bool isCorrectArgs(int argc, char **argv)
{
    if (argc < 2)
        return false;
    return true;
}

static size_t toInt(char *string)
{
    char* buffer;
    return (size_t)strtol(string, &buffer, 10);
}

static void getParams(size_t *x, size_t *y, size_t *width, size_t *height, char **argv)
{
    *x = toInt(argv[4]);
    *y = toInt(argv[5]);
    *width = toInt(argv[6]);
    *height = toInt(argv[7]);
}

int cropRotate(int argc, char **argv)
{
    Bitmap bitmap;
    FILE *file = fopen(argv[2], "rb");

    if (file == NULL)
    {
        error("Can't open the file");
        return -1;
    }

    readBitmap(&bitmap, file);
    fclose(file);
    debug("Bitmap is read");

    Bitmap result;
    size_t x, y, width, height;
    getParams(&x, &y, &width, &height, argv);
    crop(&bitmap, x, y, width, height, &result);
    clearBitmap(&bitmap);
    debug("Cropped");

    FILE *middle = fopen("../middle.bmp", "wb");
    saveBitmap(&result, middle);
    fclose(middle);

    FILE *to = fopen("../lena_228.bmp", "wb");

    Bitmap rotated;
    rotate(&result, &rotated);

    debug("Rotated");


    saveBitmap(&rotated, to);
    clearBitmap(&result);
    clearBitmap(&rotated);
    fclose(to);
    return 0;
}

int main(int argc, char **argv)
{
    if (!isCorrectArgs(argc, argv))
    {
        error("Not enough required parameters");
        return -1;
    }

    if (strcmp(argv[1], "crop-rotate") == 0)
    {
        int result = cropRotate(argc, argv);
        if (result != 0)
            return result;
    }
    else if (strcmp(argv[1], "insert") == 0)
    {

    }
    else //extract
    {

    }
    return  0;
}