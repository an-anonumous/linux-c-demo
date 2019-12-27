#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char * argv[])
{
    // ./a.out file
    if(argc != 2)
    {
        printf("Invalid input, please enter as follows:\n ./a.out  file \n ");
        exit(0);
    }

    //number of call "ls -l file*"
    int conuts = 0;

    // cmd : ls -l
    char cmd[256] = " ls -l ";
    strcat(cmd, argv[1]);
    strcat(cmd, "* \n");
    puts(cmd);                                    //ls -l makefile*

    printf(" %d \n ", conuts++);
    system(cmd);

    // carete a hard link of the input file.
    char hl[257] = {0};                           //name of hard link
    sprintf(hl, "%s.%s", argv[1], "hard");
    int res = link(argv[1], hl);                  //creat hard link
    if(res == -1)
    {
        perror("failed to create a had link of the inputed file.\n");
        exit(0);
    }

    printf(" %d \n ", conuts++);
    system(cmd);

    // char *getcwd(char *buf, size_t size);
    char pwd[1024] = {0};
    res = getcwd(pwd, 1024);
    if(res == NULL)
    {
        perror("getcwd");
        exit(-1);
    }

    //get absolute path of the input file.
    strcat(pwd, "/");
    strcat(pwd, argv[1]);
    printf("pwd = %s\n", pwd);

    //create a soft link of the specified file.
    char sl[257] = {0};//soft linke name.
    sprintf(sl, "%s.%s", argv[1], "soft");

    //  int symlink(const char *target, const char *linkpath);
    res = symlink(pwd, sl);                        //create soft link
    if(res == -1)
    {
        perror("failed to create a soft link of the inputed file.\n");
        exit(0);
    }

    printf(" %d \n ", conuts++);
    system(cmd);


    //         read link
    char path[1024] = {0};

    //ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);
    res = readlink(sl, path, 1023);
    if(res == -1)
    {
        perror("failed to readlink.\n");
        exit(-1);
    }

    printf("path of soft link %s is %s \n", sl, path);


    // unlink soft link

    // int unlink(const char *pathname);
    res =    unlink(sl);
    if(res == -1)
    {
        perror("failed to unlink soft link.\n");
        exit(-1);
    }

    printf(" %d \n ", conuts++);
    system(cmd);

    // unlink hard link
    res =    unlink(hl);
    if(res == -1)
    {
        perror("failed to unlink hard link.\n");
        exit(-1);
    }

    printf(" %d \n ", conuts++);
    system(cmd);
}
