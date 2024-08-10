#include <assert.h>
#include <stdio.h>

int main(void)
{
    char arr[108];
    char *ptr = arr;
    printf("char arr[108]; sizeof(arr)+1=%ld\nptr=arr; sizeof(ptr)=%ld\n",
            sizeof(arr)+1, sizeof(ptr));
    printf("sizeof(arr+1)=%ld\n", sizeof(arr+1));
    assert(109 == sizeof(arr)+1);
    assert(8 == sizeof(ptr));
    /* arr here is just a pointer points to the first element of the array */
    assert(8 == sizeof(arr+1));
    return 0;
}
