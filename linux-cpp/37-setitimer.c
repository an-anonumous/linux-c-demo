#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#ifdef ASDFGHJKL
struct itimerval
{
    struct timeval it_interval; /* Interval for periodic timer */
    struct timeval it_value;    /* Time until next expiration */
};
struct timeval
{
    time_t tv_sec;       /* seconds */
    suseconds_t tv_usec; /* microseconds */
};
#endif

int main()
{
    struct itimerval new, old;
    while(1)
    {
        sleep(1);

        // int setitimer(int which, const struct itimerval *new_value,struct itimerval *old_value);
        new.it_interval.tv_sec = 5;
        new.it_interval.tv_usec = 100;

        new.it_value.tv_sec = rand() % 5;
        new.it_value.tv_usec = 123;

        int res = setitimer(ITIMER_REAL, &new, &old);
        if(res == -1 )
        {
            perror("setitimer\n");
            exit(-1);
        }

        printf("old: Interval=%lds+%ldms,it-value=%lds+%lds\n", old.it_interval.tv_sec, old.it_interval.tv_usec, old.it_value.tv_sec,
               old.it_value.tv_usec);

        printf("new: Interval=%lds+%ldms,it-value=%lds+%lds\n", new.it_interval.tv_sec, new.it_interval.tv_usec, new.it_value.tv_sec,
               new.it_value.tv_usec);
    }
}
