#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

void tree(const char * rootDir);

int main(int argc, char * argv[])
{
    //         ./mytree /user/local
    if(argc != 2 )
    {
        printf(" ./mytree /user/local");
        exit(-1);
    }

    puts(argv[1]);   // output the root dir.
    tree(argv[1]);   // 开始递归遍历目录
}

int depth = 0;//递归的深度

/*
 * 递归输出指定目录下所有文件。
 */
void tree(const char * rootDir)
{
    // DIR *opendir(const char *name);
    DIR * dir = NULL;
    dir =    opendir(rootDir);
    if(dir == NULL)
    {
        perror("failed to opendir");
        exit(-1);
    }

    //  struct dirent *readdir(DIR *dirp);
    struct dirent * entry = NULL;
    char line[1024] = {0};      // output string.
    char path[1024] = {0};      // absolute full dir path.
    while(entry = readdir(dir))
    {
        // skip directory . and ..
        if(entry->d_type == DT_DIR && ( strcmp(entry->d_name, ".") == 0 ||  strcmp(entry->d_name, "..") == 0) )
        {
            continue;
        }

        // if current entry is a directory
        if(entry->d_type == DT_DIR )
        {
            depth++;

            //puts(entry->d_name);
            memset(line, 0, sizeof(line));
            for(int i = 0; i < depth; i++)
            {
                if( i == depth - 1)
                {
                    strcat(line, "|___");
                }
                else
                {
                    strcat(line, "    ");
                }
            }
            sprintf(line + strlen(line), "/%s", entry->d_name);
            puts(line);
            memset(line, 0, sizeof(line));

            // next depth dir.
            memset(path, 0, sizeof(path));
            sprintf(path, "%s/%s", rootDir, entry->d_name);
            tree(path);

            depth--;

            // void *memset(void *s, int c, size_t n);)
            memset(path, 0, sizeof(path));
        }

        //common files
        if(entry->d_type == DT_REG)
        {
            memset(line, 0, sizeof(line));
            for(int i = 0; i <= depth; i++)
            {
                //strcat(line, "|____");
                if( i == depth )
                {
                    strcat(line, "|___");
                }
                else
                {
                    strcat(line, "    ");
                }
            }
            sprintf(line + strlen(line), "%s", entry->d_name);
            puts(line);
            memset(line, 0, sizeof(line));
        }
    }//while

    int res = closedir(dir);  //close current directory.
    if(res == -1)
    {
        perror("closedir");
        exit(-1);
    }
}
