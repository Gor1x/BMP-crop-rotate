#include "stdbool.h"
#include "bmp.h"
#include "stdio.h"
#include "stego.h"

static void error(const char *type)
{
    printf("Error: %s\n", type);
}

static bool isCorrectArgs(int argc)
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
    return x + width <= bitmap->width && y + height <= bitmap->height;
}

static bool isCorrectCropRotateArgs(int argc)
{
    return argc == 8;
}

static int getBitmapFromFile(Bitmap *bitmap, char *fileName)
{
    FILE *file = fopen(fileName, "rb");

    if (file == NULL) {
        error("Can't open input file");
        return 1;
    }

    if (readBitmap(bitmap, file) != 0)
    {
        fclose(file);
        error("Memory allocation failed");
        return 7;
    }

    fclose(file);
    return 0;
}

int cropRotate(int argc, char **argv) {
    if (!isCorrectCropRotateArgs(argc)) {
        error("Not enough required parameters for crop-rotate");
        return 4;
    }

    Bitmap bitmap;
    int res = getBitmapFromFile(&bitmap, argv[2]);
    if (res != 0)
        return res;

    Bitmap result;
    size_t x, y, width, height;
    getParams(&x, &y, &width, &height, argv);

    if (!isCorrectParameters(&bitmap, x, y, width, height))
    {
        error("Parameters are incorrect");
        clearBitmap(&bitmap);
        return 3;
    }

    if (crop(&bitmap, x, y, width, height, &result) != 0)
    {
        error("Memory allocation failed");
        clearBitmap(&bitmap);
        return 7;
    }
    clearBitmap(&bitmap);

    Bitmap rotated;

    if (rotate(&result, &rotated) != 0)
    {
        clearBitmap(&result);
        error("Memory allocation failed");
        return 7;
    }
    clearBitmap(&result);

    FILE *to = fopen(argv[3], "wb");
    if (to == NULL)
    {
        clearBitmap(&rotated);
        error("Can't open output file");
        return 1;
    }

    saveBitmap(&rotated, to);
    clearBitmap(&rotated);
    fclose(to);
    return 0;
}

static bool isInsertParamsCorrect(int argc)
{
    return argc == 6;
}

int insert(int argc, char **argv)
{
    if (!isInsertParamsCorrect(argc))
    {
        error("Not enough required parameters for insert");
        return 1;
    }

    Bitmap bitmap;
    int result = getBitmapFromFile(&bitmap, argv[2]);
    if (result != 0)
        return result;

    FILE *key = fopen(argv[4], "r");
    if (key == NULL)
    {
        error("Failed to open key file");
        return 16;
    }

    FILE *message = fopen(argv[5], "r");
    if (message == NULL)
    {
        fclose(key);
        error("Failed to open message file");
        return 17;
    }

    insertStegoData(&bitmap, message, key);
    fclose(message);
    fclose(key);


    FILE *to = fopen(argv[3], "wb");
    if (to == NULL)
    {
        clearBitmap(&bitmap);
        error("Failed to open output file");
        return 18;
    }

    saveBitmap(&bitmap, to);
    fclose(to);
    clearBitmap(&bitmap);
    return 0;
}


int main(int argc, char **argv)
{
    if (!isCorrectArgs(argc))
    {
        error("Not enough required parameters");
        return 9;
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
        int result = insert(argc, argv);
        if (result != 0)
            return result;
        printf("Your secrets was hidden");
    }
    else //extract
    {

        printf("Your secrets was found");
    }
    return  0;
}