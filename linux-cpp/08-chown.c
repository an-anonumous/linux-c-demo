#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{
    // ./mychown file newuser
    if(argc != 4)
    {
        printf("invalid call format.\n please input like \" ./mychown file newuser newgroup\" \n");
    }

    //convert string number from commond line to int .
    int uid = strtol(argv[1], NULL, 10);
    int gid = strtol(argv[2], NULL, 10);
    printf("uid = %d ,gid = %d \n", uid, gid);
    if(uid == LONG_MIN || uid == LONG_MAX || gid == LONG_MIN || uid == LONG_MAX)
    {
        exit(0);
    }

    printf("before \n");
    char cmd[1024] = {0};
    sprintf(cmd, " ls -l %s \n", argv[1]);
    printf("%s\n", cmd);
    system(cmd);

    //int chown(const char *pathname, uid_t owner, gid_t group);)
    //id 用户id和组ID可以通过 id user 来查询
    //uid=0(root) gid=0(root) groups=0(root)
    int res = chown(argv[1], uid, gid);
    if(res == -1)
    {
        perror("failed to chown.\n");
        exit(-1);
    }

    printf("before \n");
    system(cmd);
}
