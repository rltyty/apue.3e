#ifndef _RLTAPUE_H
#define _RLTAPUE_H

/* simple stack */
#define push(sp, n) (*((sp)++)) = (n)
#define pop(sp) (*--(sp))

/* max and min */
#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) <= (b) ? (a) : (b))

/* print error message */
void my_perror(const char*, ...);

/* limits and constants */
#define MAXLINE 4096                                 /* max line length */

#define OPEN_MAX_GUESS 256
long open_max(void);

/* flags are file status flags to turn on */
void set_fl(int, int);

/* profiling running time */
void timeit(void(*)(), int);

#endif /* _RLTAPUE_H */
