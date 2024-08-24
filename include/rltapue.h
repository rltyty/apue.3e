#ifndef _RLTAPUE_H
#define _RLTAPUE_H

/* simple stack */
#define push(sp, n) (*((sp)++)) = (n)
#define pop(sp) (*--(sp))

/* print error message */
void my_perror(const char*, ...);

/* limits and constants */
#define MAXLINE 4096                                 /* max line length */

#define OPEN_MAX_GUESS 256
long open_max(void);

#endif /* _RLTAPUE_H */
