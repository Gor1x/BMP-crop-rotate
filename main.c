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


static bool isCorrectParameters(Bitmap *bitmap, size_t x, size_t y, size_t width, size_t height)
{
    return x >= 0 && y >= 0 && width >= 0 && height >= 0 &&
                                        x + width <= bitmap->width && y + height <= bitmap->height;
}

static bool isCorrectCropRotateArgs(int argc, char **argv)
{
    return argc == 8;
}

int cropRotate(int argc, char **argv)
{
    if (!isCorrectCropRotateArgs(argc, argv))
    {
        error("Not enough required parameters for crop-rotate");
        return 4;
    }

    Bitmap bitmap;
    FILE *file = fopen(argv[2], "rb");

    if (file == NULL)
    {
        error("Can't open input file");
        return 1;
    }

    readBitmap(&bitmap, file);
    fclose(file);

    Bitmap result;
    size_t x, y, width, height;
    getParams(&x, &y, &width, &height, argv);

    if (!isCorrectParameters(&bitmap, x, y, width, height))
    {
        error("Parameters are incorrect");
        clearBitmap(&bitmap);
        return 3;
    }

    crop(&bitmap, x, y, width, height, &result);
    clearBitmap(&bitmap);

    FILE *to = fopen(argv[3], "wb");
    if (to == NULL)
    {
        error("Can't open output file");
        clearBitmap(&result);
        return 2;
    }

    Bitmap rotated;
    rotate(&result, &rotated);
    clearBitmap(&result);

    saveBitmap(&rotated, to);

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
        printf("Done!\n");
    }
    else if (strcmp(argv[1], "insert") == 0)
    {

    }
    else //extract
    {

    }
    return  0;
}