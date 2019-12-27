#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int i = 0;
  while(1)
  {
      printf("%d\n", i++);
     if(i == 3276 ) {
        abort();              
      }

  }
}
