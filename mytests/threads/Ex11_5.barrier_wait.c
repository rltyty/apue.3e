/**
 * Ex11_5:
 * What synchronization primitives would you need to implement a barrier?
 * Provide an implementation of the pthread_barrier_wait function.
 */

#include <pthread.h>
#include <stdio.h>
#include <sys/errno.h>
#include "rltapue.h"

#define THRD_NUMS 11

typedef struct my_barrier {
  unsigned count;
  pthread_mutex_t lock;
  pthread_cond_t cond;
} my_barrier_t;

#define MY_BARRIER_SERIAL_THREAD -1
my_barrier_t barrier;

int my_barrier_init(my_barrier_t *barrier, unsigned count) {
  if (barrier == NULL || count == 0) {
    return EINVAL;
  }
  int rc;
  if ((rc = pthread_mutex_init(&barrier->lock, NULL)) != 0) return rc;
  if ((rc = pthread_cond_init(&barrier->cond, NULL)) != 0) return rc;
  barrier->count = count;
  return 0;
}

int my_barrier_wait(my_barrier_t *barrier) {
  pthread_mutex_lock(&barrier->lock);
  if (--barrier->count == 0) {
    pthread_cond_broadcast(&barrier->cond);
    pthread_mutex_unlock(&barrier->lock);
    return MY_BARRIER_SERIAL_THREAD;
  }
  while (barrier->count > 0) {
    pthread_cond_wait(&barrier->cond, &barrier->lock);
  }
  pthread_mutex_unlock(&barrier->lock);
  return 0;
}

int my_barrier_destroy(my_barrier_t *barrier) {
  int rc = 0;
  if ((rc = pthread_cond_destroy(&barrier->cond)) != 0) return rc;
  if ((rc = pthread_mutex_destroy(&barrier->lock)) != 0) return rc;
  barrier->count = 0;
  return 0;
}

void *thrd_func(void *arg) {
  char *name = arg;

  int rc = my_barrier_wait(&barrier);
  if (rc == MY_BARRIER_SERIAL_THREAD) {
    printf("I, %s, am the chosen one, now let's play!\n", name);
  } else if (rc == 0) {
    printf("%s is playing football.\n", name);
  } else {
    my_perror_ret("error: my_barrier_wait() rc=%d", rc);
  }
  return (void*)0;
}

int main(int argc, char *argv[]) {
  pthread_t lineup[THRD_NUMS];
  char *names[THRD_NUMS] = {
    "Messi", "Alvarez", "Di Maria", "Mac Allister", "Fernandez",
    "De Paul", "Tagliafico", "Otamendi", "Romero", "Molina", "Martinez"
  };

  // 0. Setup barrier
  my_barrier_init(&barrier, THRD_NUMS);

  // 1. create worker threads
  for (int i = 0; i < THRD_NUMS; i++) {
    pthread_create(&lineup[i], NULL, thrd_func, names[i]);
    printf("%s is added to lineup.\n", names[i]);
  }

  // 2. wait for all the threads
  for (int i = 0; i < THRD_NUMS; i++) {
    int rc = 0;
    if ((rc = pthread_join(lineup[i], NULL)) != 0) {
      my_perror_ret("error: pthread_join: worker[%d], rc=%d", i, rc);
    }
  }

  // 3. clean up barrier
  my_barrier_destroy(&barrier);
  return 0;
}

/*
# Test 1:
> ./Debug/threads/Ex11_5.barrier_wait
Messi is added to lineup.
Alvarez is added to lineup.
Di Maria is added to lineup.
Mac Allister is added to lineup.
Fernandez is added to lineup.
De Paul is added to lineup.
Tagliafico is added to lineup.
Otamendi is added to lineup.
Romero is added to lineup.
Molina is added to lineup.
Martinez is added to lineup.
Mac Allister is playing football.
Romero is playing football.
I, Martinez, am the chosen one, now let's play!
Molina is playing football.
De Paul is playing football.
Di Maria is playing football.
Alvarez is playing football.
Tagliafico is playing football.
Otamendi is playing football.
Messi is playing football.
Fernandez is playing football.

# Test 2:
> ./Debug/threads/Ex11_5.barrier_wait
Messi is added to lineup.
Alvarez is added to lineup.
Di Maria is added to lineup.
Mac Allister is added to lineup.
Fernandez is added to lineup.
De Paul is added to lineup.
Tagliafico is added to lineup.
Otamendi is added to lineup.
Romero is added to lineup.
Molina is added to lineup.
Martinez is added to lineup.
I, Martinez, am the chosen one, now let's play!
Di Maria is playing football.
Fernandez is playing football.
Otamendi is playing football.
Tagliafico is playing football.
Mac Allister is playing football.
Molina is playing football.
Messi is playing football.
Alvarez is playing football.
De Paul is playing football.
Romero is playing football.

# Test 3: (In Neovim)
:!Debug/threads/Ex11_5.barrier_wait
Messi is added to lineup.
Alvarez is added to lineup.
Di Maria is added to lineup.
Mac Allister is added to lineup.
Fernandez is added to lineup.
De Paul is added to lineup.
Tagliafico is added to lineup.
Otamendi is added to lineup.
Romero is added to lineup.
Molina is added to lineup.
Martinez is added to lineup.
I, Martinez, am the chosen one, now let's play!
Molina is playing football.
Tagliafico is playing football.
Fernandez is playing football.
Messi is playing football.
Mac Allister is playing football.
De Paul is playing football.
Di Maria is playing football.
Romero is playing football.
Alvarez is playing football.
Otamendi is playing football.

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
