#include <stdio.h>
#include <stdlib.h>

int main()
{
    puts(getenv("PATH"));

    int res = setenv("PATH", "hahaha", 0);
    if(res == -1)
    {
        perror("set PATH");
        exit(-1);
    }

    //system("echo $PATH");
    puts(getenv("PATH"));    //not overwrite old path.

    //int setenv(const char *name, const char *value, int overwrite);
    res = setenv("PATH", "hahaha", 1);
    if(res == -1)
    {
        perror("set PATH");
        exit(-1);
    }

    puts(getenv("PATH"));  // overrided the old PATH value with hahaha.

    // int unsetenv(const char *name);
    res =   unsetenv("PATH");
    if(res == -1)
    {
        perror("unset PATH");
        exit(-1);
    }

    char * path = getenv("PATH");
    if(path != NULL)
    {
        printf(" %s \n ", path);
    }
    else
    {
        puts("==========");   // path == NULL
    }
    //    system("echo $PATH");

    puts("**********************");

    res =    setenv("abcd", "12334565", 1);
    if(res == -1)
    {
        perror("set abcd");
        exit(-1);
    }
    printf("abcd = %s \n", getenv("abcd"));  //abcd = 12334565

    res =   unsetenv("abcd");
    if(res == -1)
    {
        perror("set abcd");
        exit(-1);
    }

    printf("abcd = %s \n", getenv("abcd"));

    res =   unsetenv("abcd");               //abcd = (null)
    if(res == -1)
    {
        perror("set abcd");
        exit(-1);
    }

    printf("abcd = %s \n", getenv("abcd"));  // abcd = (null)
}
