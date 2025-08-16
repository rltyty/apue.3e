#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>

/*
 * Difference between read from dev/tty and read from standard input.
 */
int main(int argc, char *argv[]) {
  int termfd = open("/dev/tty", O_RDONLY);
  char buf[BUFSIZ];
  int n;
  while ((n = read(termfd, buf, BUFSIZ)) > 0) { // read from terminal
    printf("Echo input from terminal:[%s]\n", buf);
  } // <CTRL-D> to end input, <CTRL-D> w/o input to end the loop 

  while ((n = read(STDIN_FILENO, buf, BUFSIZ)) > 0) { // read from STDIN
    printf("Echo input from STDIN:[%s]\n", buf);
  }
}

/*
> ./Debug/procrelation/ctrlterm
tty input 1. Echo input from terminal:[tty input 1. ]
tty input 2. Echo input from terminal:[tty input 2. ]
std input 1. Echo input from STDIN:[std input 1. ]
std input 2. Echo input from STDIN:[std input 2. ]

> ./Debug/procrelation/ctrlterm < log2
tty input 1. Echo input from terminal:[tty input 1. ]
tty input 2. Echo input from terminal:[tty input 2. ]
Echo input from STDIN:[Sat Aug 16 12:16:54 CST 2025
Main process say Hi!
#]
 */
