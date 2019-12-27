#include <stdio.h>
#include "sub.h"
#include "add.h"

int main(int argc, const char **argv)
{
    int a = 10, b = 2;

    printf("a + b = %d\r\n", add(a, b));
    printf("a - b = %d\r\n", sub(a, b));

    getchar();

    return 0;
}