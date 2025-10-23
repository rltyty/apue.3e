#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * 1. Both `puts(3)` and `printf(3)` are buffered.
 * 2. `puts(3)` automatically adds '\n' whereas `printf(3)` doesn't.
 * 3. `puts(3)` doesn't support formatting, so it's usually slightly faster
 *    than `printf(3)`
 */
void test_puts(int loop, char *msg)
{
    for (int i = 0; i < loop; i++) {
        puts(msg);
    }
}

void test_printf(int loop, char *msg)
{
    for (int i = 0; i < loop; i++) {
        printf("%s\n", msg);
    }
}

void test_printf_no_newline(int loop, char *msg)
{
    for (int i = 0; i < loop; i++) {
        printf("%s", msg);
    }
}

double profiling(void (*f)(int, char*), int loop, char* msg)
{
    clock_t start, end;
    start = clock();
    f(loop, msg);
    end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("./a.out <loop> <msg>\n");
        return 1;
    }
    int loop = atoi(argv[1]);
    char msg[1024];
    strncpy(msg, argv[2], sizeof(msg) - 1);
    msg[sizeof(msg) - 1] = '\0';


    double t1 = profiling(test_puts, loop, msg);
    double t2 = profiling(test_printf, loop, msg);
    double t3 = profiling(test_printf_no_newline, loop, msg);

    printf("Profiling test_puts: %d msg:[%s]: Time used: %f\n", loop, msg, t1);
    printf("Profiling test_printf: %d msg:[%s]: Time used: %f\n", loop, msg, t2);
    printf("Profiling test_printf_no_newline: %d msg:[%s]: Time used: %f\n", loop, msg, t3);
    return 0;
}

/*

# 1. Line-buffered (console): the first two flush their streams line by line
> ./Debug/general/puts_printf_test 100000 Messi
...
...
Profiling test_puts: 100000 msg:[Messi]: Time used: 0.096289
Profiling test_printf: 100000 msg:[Messi]: Time used: 0.109352
Profiling test_printf_no_newline: 100000 msg:[Messi]: Time used: 0.013930


# 2. Fully-buffered (redirection to normal file): '\n' doesn't trigger flush
> ./Debug/general/puts_printf_test 100000 Messi > full
> tail -3 file
...
Profiling test_puts: 100000 msg:[Messi]: Time used: 0.007696
Profiling test_printf: 100000 msg:[Messi]: Time used: 0.013391
Profiling test_printf_no_newline: 100000 msg:[Messi]: Time used: 0.011918

 */
