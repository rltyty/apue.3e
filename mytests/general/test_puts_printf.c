#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

    printf("Profiling test_puts: %d msg:[%s]: Time used: %f\n", loop, msg, t1);
    printf("Profiling test_printf: %d msg:[%s]: Time used: %f\n", loop, msg, t2);
    return 0;
}

/*
 * Ex1.
 * Profiling test_puts: 1000000 msg:[Messi]: Time used: 0.063512
 * Profiling test_printf: 1000000 msg:[Messi]: Time used: 0.115305
 */
