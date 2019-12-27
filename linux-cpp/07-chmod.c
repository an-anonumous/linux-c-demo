#include <limits.h> // LONG_MIN LONG_MAX
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("please call this process in the format like \n \r ./chmod file 777 ");
        exit(0);
    }

    //long int strtol(const char *nptr, char **endptr, int base);
    int mod = strtol(argv[2], NULL, 8);
    if (mod == LONG_MIN || mod == LONG_MAX) {
        printf("some errors occurred in the period of transform str %s to long", argv[2]);
        exit(0);
    }

    printf("file =%s , mod = %d \n", argv[1], mod);

    //int chmod(const char *pathname, mode_t mode);
    int res = chmod(argv[1], mod);
    if (res == 0) {
        printf("chmod success");
    } else if (res == -1) {
        perror("operation failed ! \n");
        exit(0);
    }
}
