#include "stego.h"


static char getSymbol(int x)
{
    if (x < 26)
        return 'A' + x;
    else if (x == 26)
        return '.';
    else if (x == 27)
        return ' ';
    else
        return ',';
}

static int getBit(const Pixel* pixel, char color)
{
    size_t pos;
    if (color == 'R')
        pos = 2;
    else if (color == 'G')
        pos = 1;
    else
        pos = 0;
    return (pixel->data[pos] & 1);
}

void extractStegoData(const Bitmap *bitmap, FILE *key, FILE *message)
{
    size_t x, y;
    char color;
    while (true)
    {
        int a = 0;

        bool isEnd = false;
        for (size_t j = 0; j < BITS_IN_SYMBOL; j++)
        {
            if (fscanf(key, "%zu %zu %c", &x, &y, &color) != 3)
            {
                isEnd = true;
                break;
            }
            a |= (getBit(&bitmap->picture[y][x], color) << j);
        }
        if (isEnd)
            break;
        fprintf(message, "%c", getSymbol(a));
    }
}

static int getCode(char x)
{
    if ('A' <= x && x <= 'Z')
        return x - 'A';
    else if (x == '.')
        return 26;
    else if (x == ' ')
        return 27;
    else //x == ','
        return 28;
}

static void setBit(Pixel *pixel, char color, int bit)
{
    size_t pos;
    if (color == 'R')
    {
        pos = 2;
    }
    else if (color == 'G')
    {
        pos = 1;
    }
    else
    {
        pos = 0;
    }

    if (bit == 0) //Need to be set
    {
        if (pixel->data[pos] % 2 != 0)
            pixel->data[pos] -= 1;
    }
    else //Need to be set 0
    {
        pixel->data[pos] |= 1;
    }
}

static int write(Bitmap *bitmap, int number, FILE *key)
{
    size_t x, y;
    char color;
    for (size_t i = 0; i < 5; i++)
    {
        int bit = number % 2;
        number /= 2;

        if (fscanf(key,"%zu %zu %c", &x, &y, &color) != 3)
        {
            printf("Error: Key is too short\n");
            return 1;
        }
        if (x >= bitmap->width || y >= bitmap->height)
        {
            printf("Error: Wrong parameters of key\n");
            return 1;
        }
        setBit(&bitmap->picture[y][x], color, bit);
    }
    return 0;
}

int insertStegoData(Bitmap *bitmap, FILE* message, FILE *key)
{
    char symbol;
    while (fscanf(message, "%c", &symbol) == 1)
    {
        int number = getCode(symbol);
        if (write(bitmap, number, key) != 0)
            return 1;
    }
    return 0;
}
