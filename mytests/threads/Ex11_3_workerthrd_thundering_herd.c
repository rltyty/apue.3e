/**
 * Ex11.3:
 * Apply the techniques shown in Figure 11.15 to the worker thread example
 * (Figures 11.1 and 11.14) to implement the worker thread function. Don’t
 * forget to update the queue_init function to initialize the condition
 * variable and change the job_insert and job_append functions to signal the
 * worker threads. What difficulties arise?
 *
 * This solution embeds the mutex and condition variable in the queue
 * structure, which means that all worker threads will wait on the same
 * condition. When there are many worker threads, it could lead to thundering
 * herd problem, whereby many threads are awaken to without work to do,
 * resulting in a waste of CPU resources and increased lock contention.
 *
 * NOTE:
 * 1. pthread_cond_wait(3): acquire mutex lock before wait condition variable.
 * 2. pthread_cond_wait(3) code idiom:
 *    pthread_mutex_lock(&lock);
      while ((condition_not_true))
        pthread_cond_wait(&cond, &lock);
      pthread_mutex_unlock(&lock);
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rltapue.h"

struct mytid {
  pthread_t tid;
  int tnum;
};

struct job {
  struct job *j_next;
  struct job *j_prev;
  // pthread_t j_tid; /* tells which thread handles this job */
  struct mytid j_tid; /* tells which thread handles this job */
                      /* ... more stuff here ... */
  int job_id;
};

struct queue {
  struct job *q_head;
  struct job *q_tail;
  pthread_mutex_t q_lock;
  pthread_cond_t q_cond;
};

#define POOL_SIZE 3

void *thrd_func(void *arg) {
  struct job *pJob = arg;
  printf("Job ID: [%d], Job Thread: [%d:0x%lx], Current Thread ID: [0x%lx]\n",
         pJob->job_id, pJob->j_tid.tnum, (unsigned long)pJob->j_tid.tid,
         (unsigned long)pthread_self());
  return (void *)0;
}

/*
 * Initialize a queue.
 */
int queue_init(struct queue *qp) {
  int err;

  qp->q_head = NULL;
  qp->q_tail = NULL;
  err = pthread_mutex_init(&qp->q_lock, NULL);
  if (err != 0) return (err);
  /* ... continue initialization ... */
  err = pthread_cond_init(&qp->q_cond, NULL);
  if (err != 0) return (err);
  return (0);
}

/*
 * Insert a job at the head of the queue.
 */
void job_insert(struct queue *qp, struct job *jp) {
  pthread_mutex_lock(&qp->q_lock);
  jp->j_next = qp->q_head;
  jp->j_prev = NULL;
  if (qp->q_head != NULL)
    qp->q_head->j_prev = jp;
  else
    qp->q_tail = jp; /* list was empty */
  qp->q_head = jp;
  pthread_cond_broadcast(&qp->q_cond);
  pthread_mutex_unlock(&qp->q_lock);
}

/*
 * Append a job on the tail of the queue.
 */
void job_append(struct queue *qp, struct job *jp) {
  pthread_mutex_lock(&qp->q_lock);
  jp->j_next = NULL;
  jp->j_prev = qp->q_tail;
  if (qp->q_tail != NULL)
    qp->q_tail->j_next = jp;
  else
    qp->q_head = jp; /* list was empty */
  qp->q_tail = jp;
  pthread_cond_broadcast(&qp->q_cond);
  pthread_mutex_unlock(&qp->q_lock);
}

/*
 * Remove the given job from a queue.
 */
void job_remove(struct queue *qp, struct job *jp) {
  if (jp == qp->q_head) {
    qp->q_head = jp->j_next;
    if (qp->q_tail == jp)
      qp->q_tail = NULL;
    else
      jp->j_next->j_prev = jp->j_prev;
  } else if (jp == qp->q_tail) {
    qp->q_tail = jp->j_prev;
    jp->j_prev->j_next = jp->j_next;
  } else {
    jp->j_prev->j_next = jp->j_next;
    jp->j_next->j_prev = jp->j_prev;
  }
}

void job_remove_atomic(struct queue *qp, struct job *jp) {
  pthread_mutex_lock(&qp->q_lock);
  job_remove(qp, jp);
  pthread_mutex_unlock(&qp->q_lock);
}

/*
 * Find a job for the given thread ID.
 */
struct job *job_find(struct queue *pQ, pthread_t tid) {
  struct job *pJob;
  for (pJob = pQ->q_head; pJob != NULL; pJob = pJob->j_next)
    if (pthread_equal(pJob->j_tid.tid, tid)) break;
  return (pJob);
}

struct job *job_find_atomic(struct queue *pQ, pthread_t tid) {
  if (pthread_mutex_lock(&pQ->q_lock) != 0) return (NULL);
  struct job *pJob = job_find_atomic(pQ, tid);
  pthread_mutex_unlock(&pQ->q_lock);
  return (pJob);
}

/*
 * Atomic find, wait and remove a job for the given thread ID
 */
struct job *job_fetch_atomic(struct queue *pQ, pthread_t tid) {
  struct job *pJob = NULL;
  if (pthread_mutex_lock(&pQ->q_lock) != 0) return (NULL);
  while ((pJob = job_find(pQ, tid)) == NULL) {
    pthread_cond_wait(&pQ->q_cond, &pQ->q_lock);
  }
  job_remove(pQ, pJob);
  pthread_mutex_unlock(&pQ->q_lock);
  return (pJob);
}

/*
 * A long running worker thread function: waiting for jobs and then handling
 */
void *worker_thrd(void *arg) {
  struct queue *pQ = arg;
  pthread_t tid = pthread_self();
  for (;;) {
    struct job *pJob = job_fetch_atomic(pQ, tid);
    if (pJob != NULL) {
      printf(
          "Job ID: [%d], Job Thread: [%d:0x%lx], Current Thread ID: [0x%lx]\n",
          pJob->job_id, pJob->j_tid.tnum, (unsigned long)pJob->j_tid.tid,
          (unsigned long)pthread_self());
      free(pJob);
    }
  }
  return (void *)0;
}

void create_jobs(struct queue *pQ, pthread_t *pool, int psize, int jcount) {
  for (int i = 1; i <= jcount; i++) {
    // struct job job;    // !!! Classic ERROR on data lifetime, auto in loop
    struct job *pJob = malloc(sizeof(struct job)); // !!! USE dynamic alloc
    if (pJob == NULL) {
      my_perror("error: malloc() struct job");
    }
    pJob->j_tid.tid = pool[i % psize];
    pJob->j_tid.tnum = i % psize;
    pJob->job_id = i;
    if (i % 2 == 0) {
      job_append(pQ, pJob);
    } else {
      job_insert(pQ, pJob);
    }
  }
}

int main(int argc, char *argv[]) {
  // 1. Initialize job queue
  struct queue q;
  if (queue_init(&q) != 0) {
    my_perror("error: queue_init");
  }
  // 2. Launch all the worker threads in the pool
  pthread_t pool[POOL_SIZE];
  for (int i = 0; i < POOL_SIZE; i++) {
    int err = pthread_create(&pool[i], NULL, worker_thrd, &q);
    if (err) my_perror("error: pthread_create worker[%d], rc=%d", i, err);
  }

  // 3. Add jobs to the queue for execution
  printf("\nMainThread: Add 8 jobs.\n");
  create_jobs(&q, pool, POOL_SIZE, 8);
  sleep(3);
  printf("\nMainThread: Add 9 jobs.\n");
  create_jobs(&q, pool, POOL_SIZE, 9);
  sleep(3);
  printf("\nMainThread: Add 7 jobs.\n");
  create_jobs(&q, pool, POOL_SIZE, 7);
  sleep(3);
  printf("\nMainThread: Add 14 jobs.\n");
  create_jobs(&q, pool, POOL_SIZE, 14);

  pause();
  return 0;
}

/*

> ./Debug/threads/Ex11_3_workerthrd

MainThread: Add 8 jobs.
Job ID: [5], Job Thread: [2:0x70000c57a000], Current Thread ID: [0x70000c57a000]
Job ID: [2], Job Thread: [2:0x70000c57a000], Current Thread ID: [0x70000c57a000]
Job ID: [8], Job Thread: [2:0x70000c57a000], Current Thread ID: [0x70000c57a000]
Job ID: [3], Job Thread: [0:0x70000c474000], Current Thread ID: [0x70000c474000]
Job ID: [6], Job Thread: [0:0x70000c474000], Current Thread ID: [0x70000c474000]
Job ID: [7], Job Thread: [1:0x70000c4f7000], Current Thread ID: [0x70000c4f7000]
Job ID: [1], Job Thread: [1:0x70000c4f7000], Current Thread ID: [0x70000c4f7000]
Job ID: [4], Job Thread: [1:0x70000c4f7000], Current Thread ID: [0x70000c4f7000]

MainThread: Add 9 jobs.
Job ID: [5], Job Thread: [2:0x70000c57a000], Current Thread ID: [0x70000c57a000]
Job ID: [7], Job Thread: [1:0x70000c4f7000], Current Thread ID: [0x70000c4f7000]
Job ID: [9], Job Thread: [0:0x70000c474000], Current Thread ID: [0x70000c474000]
Job ID: [1], Job Thread: [1:0x70000c4f7000], Current Thread ID: [0x70000c4f7000]
Job ID: [2], Job Thread: [2:0x70000c57a000], Current Thread ID: [0x70000c57a000]
Job ID: [8], Job Thread: [2:0x70000c57a000], Current Thread ID: [0x70000c57a000]
Job ID: [4], Job Thread: [1:0x70000c4f7000], Current Thread ID: [0x70000c4f7000]
Job ID: [3], Job Thread: [0:0x70000c474000], Current Thread ID: [0x70000c474000]
Job ID: [6], Job Thread: [0:0x70000c474000], Current Thread ID: [0x70000c474000]

MainThread: Add 7 jobs.
Job ID: [5], Job Thread: [2:0x70000c57a000], Current Thread ID: [0x70000c57a000]
Job ID: [2], Job Thread: [2:0x70000c57a000], Current Thread ID: [0x70000c57a000]
Job ID: [7], Job Thread: [1:0x70000c4f7000], Current Thread ID: [0x70000c4f7000]
Job ID: [1], Job Thread: [1:0x70000c4f7000], Current Thread ID: [0x70000c4f7000]
Job ID: [4], Job Thread: [1:0x70000c4f7000], Current Thread ID: [0x70000c4f7000]
Job ID: [3], Job Thread: [0:0x70000c474000], Current Thread ID: [0x70000c474000]
Job ID: [6], Job Thread: [0:0x70000c474000], Current Thread ID: [0x70000c474000]

MainThread: Add 14 jobs.
Job ID: [11], Job Thread: [2:0x70000c57a000], Current Thread ID: [0x70000c57a000]
Job ID: [5], Job Thread: [2:0x70000c57a000], Current Thread ID: [0x70000c57a000]
Job ID: [2], Job Thread: [2:0x70000c57a000], Current Thread ID: [0x70000c57a000]
Job ID: [9], Job Thread: [0:0x70000c474000], Current Thread ID: [0x70000c474000]
Job ID: [7], Job Thread: [1:0x70000c4f7000], Current Thread ID: [0x70000c4f7000]
Job ID: [13], Job Thread: [1:0x70000c4f7000], Current Thread ID: [0x70000c4f7000]
Job ID: [1], Job Thread: [1:0x70000c4f7000], Current Thread ID: [0x70000c4f7000]
Job ID: [4], Job Thread: [1:0x70000c4f7000], Current Thread ID: [0x70000c4f7000]
Job ID: [10], Job Thread: [1:0x70000c4f7000], Current Thread ID: [0x70000c4f7000]
Job ID: [3], Job Thread: [0:0x70000c474000], Current Thread ID: [0x70000c474000]
Job ID: [6], Job Thread: [0:0x70000c474000], Current Thread ID: [0x70000c474000]
Job ID: [12], Job Thread: [0:0x70000c474000], Current Thread ID: [0x70000c474000]
Job ID: [8], Job Thread: [2:0x70000c57a000], Current Thread ID: [0x70000c57a000]
Job ID: [14], Job Thread: [2:0x70000c57a000], Current Thread ID: [0x70000c57a000]

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
