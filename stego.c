#include "stego.h"

static char getSymbol(int x)
{
    if (x < 26)
        return 'a' + x;
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
        for (size_t j = 0; j < 5; j++)
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
    if ('a' <= x && x <= 'z')
        return x - 'a';
    else if (x == '.')
        return 26;
    else if (x == ' ')
        return 27;
    else //x == ','
        return 28;
}

static int setBit(Pixel *pixel, char color, int bit)
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

    if (bit == 0)
    {
        if (pixel->data[pos] % 2 != 0)
            pixel->data[pos] -= 1;
    }
    else
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
            return 1;
        }
        if (x >= bitmap->width || y >= bitmap->height)
        {
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