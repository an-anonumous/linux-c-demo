#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>

#define nmatch (100)

int main()
{
    char str_src[] = "10.168.57.226.223.4.65.7.3.2.6.7.543.2.2.192.168.56.1.0";
    // char str_reg[] = "((([1-2][0-5]{2})|([1-9][0-9])|([0-9]))\\.){3}(([1-2][0-5]{2})|([1-9][0-9])|([0-9]))";
    char str_reg[] = "(((25[0-5])|(2[0-4][0-9])|(1[0-9]{2})|([1-9][0-9])|([0-9]))\\.){3}((25[0-5])|(2[0-4][0-9])|(1[0-9]{2})|([1-9][0-9])|([0-9]))";
    char errmsg[1024] = {0};
    char line[1024] = "";
    regmatch_t pmatch[nmatch];

    regex_t compiled_regex;
    // int regcomp(regex_t *preg, const char *regex, int cflags);
    int ret = regcomp(&compiled_regex, str_reg, REG_EXTENDED | REG_NEWLINE);
    if (ret != 0)
    {
        printf("failled to compile regex %s ", str_reg);
        return -1;
    }

    int j = 0;
    while (j < strlen(str_src) - 1 - 8)
    {
        // int regexec(const regex_t *preg, const char *string, size_t nmatch, regmatch_t pmatch[], int eflags);
        ret = regexec(&compiled_regex, str_src + j, nmatch, pmatch, 0);
        if (ret == REG_NOMATCH)
        {
            printf("%s::%d %s error", __FILE__, __LINE__, __func__);

            // size_t regerror(int errcode, const regex_t *preg, char *errbuf, size_t errbuf_size);
            size_t size = regerror(ret, &compiled_regex, errmsg, 1024);
            if (size >= 1024)
            {
                errmsg[1023] = '\0';
            }
            printf("%s", errmsg);
            return -1;
        }

        // typedef struct
        // {
        //     regoff_t rm_so; /* Byte offset from string's start to substring's start.  */
        //     regoff_t rm_eo; /* Byte offset from string's start to substring's end.  */
        // } regmatch_t;

        // The offsets of the subexpression starting at the ith open parenthesis are stored in pmatch[i].
        // The entire regular expression's match addresses are stored in pmatch[0].
        // (Note that to return the offsets of N subexpression matches, nmatch must be at least N+1.)
        //  Any unused structure elements will contain the value -1.
        strncpy(line, str_src + pmatch[0].rm_so + j, pmatch[0].rm_eo - pmatch[0].rm_so);
        puts(line);
        memset(line, 0, sizeof(line));
        memset(pmatch, 0, sizeof(pmatch));
        // for (int i = 0; pmatch[i].rm_eo != -1; i++)
        // {
        //     // char *strncpy(char *restrict s1, const char *restrict s2, size_t n)
        //     strncpy(line, str_src + pmatch[i].rm_so, pmatch[i].rm_eo - pmatch[i].rm_so);
        //     puts(line);
        //     memset(line, 0, sizeof(line));
        // }
        j++;
    }
    // void regfree(regex_t *preg);
    regfree(&compiled_regex);

    return 0;
}

// "10.168.57.226.223.4.65.7.3.2.6.7.543.2.2.192.168.56.1.0"
// 10.168.57.226
// 0.168.57.226
// 168.57.226.223
// 168.57.226.223
// 68.57.226.223
// 8.57.226.223
// 57.226.223.4
// 57.226.223.4
// 7.226.223.4
// 226.223.4.65
// 226.223.4.65
// 26.223.4.65
// 6.223.4.65
// 223.4.65.7
// 223.4.65.7
// 23.4.65.7
// 3.4.65.7
// 4.65.7.3
// 4.65.7.3
// 65.7.3.2
// 65.7.3.2
// 5.7.3.2
// 7.3.2.6
// 7.3.2.6
// 3.2.6.7
// 3.2.6.7
// 2.6.7.54
// 2.6.7.54
// 43.2.2.192
// 43.2.2.192
// 43.2.2.192
// 43.2.2.192
// 43.2.2.192
// 43.2.2.192
// 43.2.2.192
// 3.2.2.192
// 2.2.192.168
// 2.2.192.168
// 2.192.168.56
// 2.192.168.56
// 192.168.56.1
// 192.168.56.1
// 92.168.56.1
// 2.168.56.1
// 168.56.1.0
// 168.56.1.0
