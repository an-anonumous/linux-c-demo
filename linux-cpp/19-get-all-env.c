#include <stdio.h>

extern char ** environ;

/*
 *get all env
 */
int main()
{
    for(int i = 0; environ[i]; i++)
    {
        puts(environ[i]);
    }
}
