#include <stdio.h>

extern long my_open_max();

int main(void)
{
    printf("open_max of current process is %ld\n", my_open_max());
    return 0;
}
