#ifndef _RLTAPUE_H
#define _RLTAPUE_H
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

/* simple stack */
#define push(sp, n) (*((sp)++)) = (n)
#define pop(sp) (*--(sp))

/* max and min */
#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) <= (b) ? (a) : (b))

/* print error message */
void my_perror_ret(const char *, ...);
void my_perror(const char *, ...);
void my_perr_dump(const char *, ...);

/* limits and constants */
#define MAXLINE 4096 /* max line length */

#define OPEN_MAX_GUESS 256
long open_max(void);

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DIR_MODE (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

/* flags are file status flags to turn on */
void set_fl(int, int);

/* profiling running time */
void timeit(void (*)(), int);

/* get file type */
int get_ft(const char *, char **);

/* print exit staus */
void pr_exit(int);

/* print environ */
void pr_environ(void);

/* Append a path to the PATH environment */
int add_path(char *);

/* print separator line */
void pr_sep(char, int);
void pr_sep_msg(char, int, char *);

/* print process status and signal received */
void pr_statsig(int);

/* for signal handlers */
typedef void Sigfunc(int);

#define SIG_NAME_SET_SIZE 512
void pr_mask(const char *);
void pr_mask2(const char *, sigset_t *);
void pr_mask3(const char *, sigset_t);

/* convert pid_t to string */
char *pidstr(pid_t);

/* show ps info */
void ps(pid_t);

/* {App misc_source} */
void err_msg(const char *, ...);
void err_dump(const char *, ...) __attribute__((noreturn));
void err_quit(const char *, ...) __attribute__((noreturn));
void err_cont(int, const char *, ...);
void err_exit(int, const char *, ...) __attribute__((noreturn));
void err_ret(const char *, ...);
void err_sys(const char *, ...) __attribute__((noreturn));

/* parent/child from {Sec race_conditions} */
void TELL_WAIT(void);
void TELL_PARENT(pid_t);
void TELL_CHILD(pid_t);
void WAIT_PARENT(void);
void WAIT_CHILD(void);

/* Core bit operations */
uint32_t bit_set(uint32_t value, uint8_t n);
uint32_t bit_clear(uint32_t value, uint8_t n);
uint32_t bit_toggle(uint32_t value, uint8_t n);
bool bit_test(uint32_t value, uint8_t n);
/* LSB operations */
uint32_t bit_lsb_value(uint32_t value);  // Returns mask of lowest set bit
int8_t bit_lsb_position(uint32_t value); // Returns position (-1 if none)
uint32_t bit_clear_lsb(uint32_t value);  // Removes lsb, returns what remains.
/* Alignment operations */
uint32_t bit_align_up(uint32_t value, uint32_t alignment);
uint32_t bit_align_down(uint32_t value, uint32_t alignment);
// Utility functions
bool bit_is_power_of_two(uint32_t value);
uint8_t bit_count(uint32_t value);  // Population count

#endif /* _RLTAPUE_H */
