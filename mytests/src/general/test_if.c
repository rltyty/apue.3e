#include <stdio.h>

int main(void)
{
    int a = 1;
    if (a) {
        printf("if(1) is equivalent to if(true)\n");
    } else {
        printf("if(1) is equivalent to if(false)\n");
    }
    return 0;
}
