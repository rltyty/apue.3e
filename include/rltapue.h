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

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DIR_MODE (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

/* flags are file status flags to turn on */
void set_fl(int, int);

/* profiling running time */
void timeit(void(*)(), int);

/* get file type */
int get_ft(const char*, char**);

/* print exit staus */
void pr_exit(int);

/* print environ */
void pr_environ(void);

/* Append a path to the PATH environment */
int add_path(char *);

/* print separator line */
void pr_sep(char, int);
void pr_sep_msg(char, int, char *);

#endif /* _RLTAPUE_H */
