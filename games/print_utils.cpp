#include <iostream>
#include "printf.h"

void PrintRepeatedly(std::ostream &out, const char *str, int times)
{
    for (int i = times - 1; i >= 0; i--)
    {
        out << str;
    }
}
