/**
 * Ex11.3:
 * Apply the techniques shown in Figure 11.15 to the worker thread example
 * (Figures 11.1 and 11.14) to implement the worker thread function. Don’t
 * forget to update the queue_init function to initialize the condition
 * variable and change the job_insert and job_append functions to signal the
 * worker threads. What difficulties arise?
 *
 * This solution uses a per-thread condition structure, so that each worker
 * thread is awakened only when there is a job for it to process.
 *
 * NOTE:
 * 1. Why pthread_cond_wait() needs a mutex?
 *    - A condition variable does not carry state by itself. It only lets
 *    threads wait for a “condition” to become true.
 *    - The mutex protects the shared condition/state that threads are
 *    checking or modifying.
 *    - Without the mutex, a race can occur: the signaling thread might change
 *    the condition and signal before the waiting thread actually goes to
 *    sleep — causing the waiting thread to miss the wakeup.
 *    safely gone to sleep.
 * 2. Typical usage pattern (pthread_cond_wait(3) code idiom):
 *    pthread_mutex_lock(&lock);
 *    while (!condition)
 *      pthread_cond_wait(&cond, &lock);
 *    pthread_mutex_unlock(&lock);
 *    The while loop surround the wait protects against spurious wakeups.
 * 3. Typical usage pattern (pthread_cond_signal(3) code idiom):
 *    pthread_mutex_lock(&lock);
 *    condition = true
 *    pthread_mutex_unlock(&lock);
 *    pthread_cond_signal(&cond);
 * 4. In this implementation, I use two locks: a reader-writer lock to
 *    guard job queue operation (insert/append/remove/find); a worker lock
 *    to guard per-worker condition variable.
 *
 * TODO:
 * 1. Use per-thread sub-queue for large mount of jobs. One large queue and
 *    O(n) of pthread_equal(3) is not efficient.
 * 2. Use sigsuspend(2) to replace pause(3)
 * 3. Use sigaction(2) to replace signal(3)
 */

#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rltapue.h"

struct workerthrd {
  pthread_t tid;
  int wid;
  pthread_mutex_t lock;
  pthread_cond_t ready;
};

struct workerarg {
  struct workerthrd *worker;
  struct queue *jobQ;
};

struct job {
  int j_id;
  struct workerthrd *j_worker;
  struct job *j_next;
  struct job *j_prev;
  /* ... more stuff here ... */
};

struct queue {
  struct job *q_head;
  struct job *q_tail;
  pthread_rwlock_t q_lock;
};

atomic_bool graceful_stop = false;
#define POOL_SIZE 3

void handle_sigint(int signo) {
  graceful_stop = true;
  printf("\n[MainThread] SIGINT received, shutting down...\n");
}

/*
 * Initialize a queue.
 */
int queue_init(struct queue *qp) {
  int err;

  qp->q_head = NULL;
  qp->q_tail = NULL;
  err = pthread_rwlock_init(&qp->q_lock, NULL);
  if (err != 0) return (err);
  /* ... continue initialization ... */
  return (0);
}

/*
 * Insert a job at the head of the queue.
 * In this implementation, condition signaling is put after reader-writer lock
 * is unlocked, for a job is worker thread specific and no other thread remove
 * the job.
 */
void job_insert(struct queue *qp, struct job *jp) {
  pthread_rwlock_wrlock(&qp->q_lock);
  jp->j_prev = NULL;
  jp->j_next = qp->q_head;
  if (qp->q_head)
    qp->q_head->j_prev = jp;
  else
    qp->q_tail = jp; /* list was empty */
  qp->q_head = jp;
  pthread_rwlock_unlock(&qp->q_lock);
  pthread_cond_signal(&jp->j_worker->ready);
}

/*
 * Append a job on the tail of the queue.
 * In this implementation, condition signaling is put after reader-writer lock
 * is unlocked, for a job is worker thread specific and no other thread remove
 * the job.
 */
void job_append(struct queue *qp, struct job *jp) {
  pthread_rwlock_wrlock(&qp->q_lock);
  jp->j_next = NULL;
  jp->j_prev = qp->q_tail;
  if (qp->q_tail)
    qp->q_tail->j_next = jp;
  else
    qp->q_head = jp; /* list was empty */
  qp->q_tail = jp;
  pthread_rwlock_unlock(&qp->q_lock);
  pthread_cond_signal(&jp->j_worker->ready);
}

/*
 * Remove the given job from a queue. Simpler, no duplication logic.
 */
void job_remove(struct queue *qp, struct job *jp) {
  pthread_rwlock_wrlock(&qp->q_lock);
  if (jp->j_prev)
    jp->j_prev->j_next = jp->j_next;
  else
    qp->q_head = jp->j_next;

  if (jp->j_next)
    jp->j_next->j_prev = jp->j_prev;
  else
    qp->q_tail = jp->j_prev;
  pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Find a job for the given thread ID.
 */
struct job *job_find(struct queue *pQ, pthread_t tid) {
  struct job *pJob = NULL;
  if (pthread_rwlock_rdlock(&pQ->q_lock) != 0) return (NULL);
  for (pJob = pQ->q_head; pJob != NULL; pJob = pJob->j_next)
    if (pthread_equal(pJob->j_worker->tid, tid)) break;
  pthread_rwlock_unlock(&pQ->q_lock);
  return (pJob);
}

/*
 * Atomic find, wait and remove a job for the given thread ID
 */
struct job *job_fetch_atomic(struct queue *pQ, struct workerthrd *worker) {
  struct job *pJob = NULL;
  pthread_mutex_lock(&worker->lock);
  while ((pJob = job_find(pQ, worker->tid)) == NULL) {
    pthread_cond_wait(&worker->ready, &worker->lock);
    if (graceful_stop) {
      pJob = NULL;
      break;
    }
  }
  if (pJob != NULL) {
    job_remove(pQ, pJob);
  }
  pthread_mutex_unlock(&worker->lock);
  return (pJob);
}

/*
 * A long-running worker thread function: waits for and handles jobs.
 */
void *worker_func(void *arg) {
  struct workerarg *warg = arg;
  struct queue *jobQ = warg->jobQ;
  struct workerthrd *worker = warg->worker;
  free(arg);
  for (;;) {
    if (graceful_stop) break;
    struct job *pJob = job_fetch_atomic(jobQ, worker);
    if (pJob != NULL) {
      printf(
          "Job ID: [%d], Worker Thread: [%d:0x%lx], Current Thread ID: "
          "[0x%lx]\n",
          pJob->j_id, pJob->j_worker->wid, (unsigned long)pJob->j_worker->tid,
          (unsigned long)pthread_self());
      free(pJob);
    }
  }
  return (void *)0;
}

void create_jobs(struct queue *pQ, struct workerthrd *workers, int psize,
                 int jcount) {
  for (int i = 1; i <= jcount; i++) {
    // struct job job;    // !!! Classic ERROR on data lifetime, auto in loop
    struct job *pJob = malloc(sizeof(struct job));  // !!! USE dynamic alloc
    if (pJob == NULL) {
      my_perror("error: malloc() struct job");
    }
    pJob->j_id = i;
    pJob->j_worker = &workers[i % psize];
    if (i % 2 == 0) {
      job_append(pQ, pJob);
    } else {
      job_insert(pQ, pJob);
    }
  }
}

int main(int argc, char *argv[]) {
  // 0. Register disposition for exit cleanup work
  signal(SIGINT, handle_sigint);

  // 1. Initialize job queue
  struct queue q;
  if (queue_init(&q) != 0) {
    my_perror("error: queue_init");
  }
  // 2. Launch all the worker threads in the pool
  struct workerthrd workers[POOL_SIZE];
  for (int i = 0; i < POOL_SIZE; i++) {
    // wid
    workers[i].wid = i;
    // mutex
    int err = pthread_mutex_init(&workers[i].lock, NULL);
    if (err != 0) my_perror("error: pthread_mutex_init() rc = %d", err);
    // condition
    err = pthread_cond_init(&workers[i].ready, NULL);
    if (err != 0) my_perror("error: pthread_cond_init() rc = %d", err);
    // tid and arg
    struct workerarg *warg = malloc(sizeof(struct workerarg));
    if (warg == NULL) my_perror("error: malloc() struct workerarg");
    warg->worker = &workers[i];
    warg->jobQ = &q;
    err = pthread_create(&workers[i].tid, NULL, worker_func, warg);
    if (err) my_perror("error: pthread_create worker[%d], rc=%d", i, err);
  }

  // 3. Add jobs to the queue for execution
  printf("\n[MainThread]: Add 8 jobs.\n");
  create_jobs(&q, workers, POOL_SIZE, 8);
  sleep(3);
  printf("\n[MainThread]: Add 9 jobs.\n");
  create_jobs(&q, workers, POOL_SIZE, 9);
  sleep(3);
  printf("\n[MainThread]: Add 7 jobs.\n");
  create_jobs(&q, workers, POOL_SIZE, 7);
  sleep(3);
  printf("\n[MainThread]: Add 14 jobs.\n");
  create_jobs(&q, workers, POOL_SIZE, 14);

  pause();

  if (graceful_stop) {
    for (int i = 0; i < POOL_SIZE; i++) { // wake up all workers
      pthread_mutex_lock(&workers[i].lock);
      pthread_cond_signal(&workers[i].ready);
      pthread_mutex_unlock(&workers[i].lock);
    }
    for (int i = 0; i < POOL_SIZE; i++) { // join all the workers
      pthread_join(workers[i].tid, NULL);
    }
    for (int i = 0; i < POOL_SIZE; i++) { // clean all the workers
      pthread_mutex_destroy(&workers[i].lock);
      pthread_cond_destroy(&workers[i].ready);
    }
    struct job *pJob = q.q_head;
    while (pJob) {                        // clean up all jobs if any remain
      job_remove(&q, pJob);
      struct job *next = pJob->j_next;
      free(pJob);
      pJob = next;
    }
    pthread_rwlock_destroy(&q.q_lock);
  }

  printf("[MainThread] All workers exited. Cleanup done.\n");
  return 0;
}

/*

> ./Debug/threads/Ex11_3_workerthrd

[MainThread]: Add 8 jobs.
Job ID: [7], Worker Thread: [1:0x70000a65b000], Current Thread ID: [0x70000a65b000]
Job ID: [1], Worker Thread: [1:0x70000a65b000], Current Thread ID: [0x70000a65b000]
Job ID: [3], Worker Thread: [0:0x70000a5d8000], Current Thread ID: [0x70000a5d8000]
Job ID: [5], Worker Thread: [2:0x70000a6de000], Current Thread ID: [0x70000a6de000]
Job ID: [4], Worker Thread: [1:0x70000a65b000], Current Thread ID: [0x70000a65b000]
Job ID: [6], Worker Thread: [0:0x70000a5d8000], Current Thread ID: [0x70000a5d8000]
Job ID: [2], Worker Thread: [2:0x70000a6de000], Current Thread ID: [0x70000a6de000]
Job ID: [8], Worker Thread: [2:0x70000a6de000], Current Thread ID: [0x70000a6de000]

[MainThread]: Add 9 jobs.
Job ID: [7], Worker Thread: [1:0x70000a65b000], Current Thread ID: [0x70000a65b000]
Job ID: [5], Worker Thread: [2:0x70000a6de000], Current Thread ID: [0x70000a6de000]
Job ID: [9], Worker Thread: [0:0x70000a5d8000], Current Thread ID: [0x70000a5d8000]
Job ID: [2], Worker Thread: [2:0x70000a6de000], Current Thread ID: [0x70000a6de000]
Job ID: [8], Worker Thread: [2:0x70000a6de000], Current Thread ID: [0x70000a6de000]
Job ID: [1], Worker Thread: [1:0x70000a65b000], Current Thread ID: [0x70000a65b000]
Job ID: [4], Worker Thread: [1:0x70000a65b000], Current Thread ID: [0x70000a65b000]
Job ID: [3], Worker Thread: [0:0x70000a5d8000], Current Thread ID: [0x70000a5d8000]
Job ID: [6], Worker Thread: [0:0x70000a5d8000], Current Thread ID: [0x70000a5d8000]

[MainThread]: Add 7 jobs.
Job ID: [7], Worker Thread: [1:0x70000a65b000], Current Thread ID: [0x70000a65b000]
Job ID: [1], Worker Thread: [1:0x70000a65b000], Current Thread ID: [0x70000a65b000]
Job ID: [4], Worker Thread: [1:0x70000a65b000], Current Thread ID: [0x70000a65b000]
Job ID: [3], Worker Thread: [0:0x70000a5d8000], Current Thread ID: [0x70000a5d8000]
Job ID: [6], Worker Thread: [0:0x70000a5d8000], Current Thread ID: [0x70000a5d8000]
Job ID: [5], Worker Thread: [2:0x70000a6de000], Current Thread ID: [0x70000a6de000]
Job ID: [2], Worker Thread: [2:0x70000a6de000], Current Thread ID: [0x70000a6de000]

[MainThread]: Add 14 jobs.
Job ID: [9], Worker Thread: [0:0x70000a5d8000], Current Thread ID: [0x70000a5d8000]
Job ID: [13], Worker Thread: [1:0x70000a65b000], Current Thread ID: [0x70000a65b000]
Job ID: [7], Worker Thread: [1:0x70000a65b000], Current Thread ID: [0x70000a65b000]
Job ID: [11], Worker Thread: [2:0x70000a6de000], Current Thread ID: [0x70000a6de000]
Job ID: [5], Worker Thread: [2:0x70000a6de000], Current Thread ID: [0x70000a6de000]
Job ID: [1], Worker Thread: [1:0x70000a65b000], Current Thread ID: [0x70000a65b000]
Job ID: [4], Worker Thread: [1:0x70000a65b000], Current Thread ID: [0x70000a65b000]
Job ID: [2], Worker Thread: [2:0x70000a6de000], Current Thread ID: [0x70000a6de000]
Job ID: [3], Worker Thread: [0:0x70000a5d8000], Current Thread ID: [0x70000a5d8000]
Job ID: [6], Worker Thread: [0:0x70000a5d8000], Current Thread ID: [0x70000a5d8000]
Job ID: [12], Worker Thread: [0:0x70000a5d8000], Current Thread ID: [0x70000a5d8000]
Job ID: [10], Worker Thread: [1:0x70000a65b000], Current Thread ID: [0x70000a65b000]
Job ID: [8], Worker Thread: [2:0x70000a6de000], Current Thread ID: [0x70000a6de000]
Job ID: [14], Worker Thread: [2:0x70000a6de000], Current Thread ID: [0x70000a6de000]
^C
[MainThread] SIGINT received, shutting down...
[MainThread] All workers exited. Cleanup done.

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
