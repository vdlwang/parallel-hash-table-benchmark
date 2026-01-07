//#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define WORK_OUT 100
#define WORK_IN 4
#define NUM_SHARDS 8

/* =========================
* Configuration
* ========================= */

typedef struct {
  int num_threads;
  int table_size;
  int duration_sec;
  unsigned int seed;
} config_t;

typedef struct {
  long value;
} bucket_t;

typedef struct {
    bucket_t *buckets;
    pthread_mutex_t locks[NUM_SHARDS];
} hash_table_t;

typedef struct {
  int thread_id;
  long ops_completed;
  unsigned int rng_state;
} thread_state_t;

/* =========================
* Globals
* ========================= */

static config_t config;
static hash_table_t table;
static volatile int stop_flag = 0;

static inline int shard_for_key(int key) {
    return key % NUM_SHARDS;
}

static inline void do_work(int iters) {
    volatile uint64_t x = 0xdeadbeef;
    for (int i = 0; i < iters; i++) {
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
    }
}

/* =========================
* Worker Thread
* ========================= */

static void *worker_thread(void *arg) {
  thread_state_t *ts = (thread_state_t *)arg;
  while (!stop_flag) {

    do_work(WORK_OUT);
    int key = rand_r(&ts->rng_state) % config.table_size;
    int shard = shard_for_key(key);

    pthread_mutex_lock(&table.locks[shard]);

    do_work(WORK_IN);
    table.buckets[key].value++;
    
    pthread_mutex_unlock(&table.locks[shard]);

    ts->ops_completed += WORK_IN;
  }

  return NULL;
}

/* =========================
* Argument Parsing
* ========================= */

static void usage(const char *prog) {
  fprintf(stderr,
      "Usage: %s [-t threads] [-s table_size] [-d duration] [-r seed]\n"
      "Defaults:\n"
      "  threads    = 1\n"
      "  table_size = 1048576 (1M)\n"
      "  duration   = 10 seconds\n"
      "  seed       = time(NULL)\n",
      prog);
  exit(EXIT_FAILURE);
}


/* =========================
* Utility
* ========================= */

static void die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

static long now_ns(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}

static void parse_args(int argc, char **argv) {
  config.num_threads = 1;
  config.table_size = 1024 * 1024;
  config.duration_sec = 10;
  config.seed = (unsigned int)time(NULL);

  int opt;
  while ((opt = getopt(argc, argv, "t:s:d:r:h")) != -1) {
    switch (opt) {
    case 't':
      config.num_threads = atoi(optarg);
      break;
    case 's':
      config.table_size = atoi(optarg);
      break;
    case 'd':
      config.duration_sec = atoi(optarg);
      break;
    case 'r':
      config.seed = (unsigned int)atoi(optarg);
      break;
    case 'h':
    default:
      usage(argv[0]);
    }
  }

  if (config.num_threads <= 0 || config.table_size <= 0 || config.duration_sec <= 0)
      usage(argv[0]);
}

/* =========================
* Initialization
* ========================= */

static void init_table(void) {
  table.buckets = (bucket_t*)calloc(config.table_size, sizeof(bucket_t));
  if (!table.buckets)
      die("calloc");

  for (int i = 0; i < NUM_SHARDS; i++) {
    if (pthread_mutex_init(&table.locks[i], NULL) != 0)
        die("pthread_mutex_init");
  }
}

static void free_table(void) {
  for (int i = 0; i < NUM_SHARDS; i++) {
    pthread_mutex_destroy(&table.locks[i]);
}
  free(table.buckets);
}

/* =========================
* Main
* ========================= */

int main(int argc, char **argv) {
  parse_args(argc, argv);
  init_table();

  pthread_t *threads = (pthread_t*)calloc(config.num_threads, sizeof(pthread_t));
  thread_state_t *states = (thread_state_t*)calloc(config.num_threads, sizeof(thread_state_t));
  if (!threads || !states)
      die("calloc");

  for (int i = 0; i < config.num_threads; i++) {
      states[i].thread_id = i;
      states[i].ops_completed = 0;
      states[i].rng_state = config.seed ^ (i * 0x9e3779b9);

      if (pthread_create(&threads[i], NULL, worker_thread, &states[i]) != 0)
          die("pthread_create");
  }

  long start_ns = now_ns();
  sleep(config.duration_sec);
  stop_flag = 1;

  for (int i = 0; i < config.num_threads; i++) {
      pthread_join(threads[i], NULL);
  }
  long end_ns = now_ns();

  long total_ops = 0;
  for (int i = 0; i < config.num_threads; i++) {
      total_ops += states[i].ops_completed;
  }

  double elapsed_sec = (end_ns - start_ns) / 1e9;
  double throughput = total_ops / elapsed_sec;

  printf(
      "threads=%d table=%d duration=%.2f ops=%ld ops_per_sec=%.2f\n",
      config.num_threads,
      config.table_size,
      elapsed_sec,
      total_ops,
      throughput
  );

  free(threads);
  free(states);
  free_table();
  return 0;
}

