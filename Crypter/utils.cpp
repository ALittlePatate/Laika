#include "utils.hpp"
#include <Windows.h>
#include <string.h>
#include <stdio.h>

void StringReplace(char* in, const char* to_find, const char* replacement)
{
    char buffer[1024];
    char* p;

    if (!(p = strstr(in, to_find)))
        return;

    strncpy(buffer, in, p - in);
    buffer[p - in] = '\0';
    sprintf(buffer + (p - in), "%s%s", replacement, p + strlen(to_find));
    strcpy(in, buffer);
}