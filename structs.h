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
  pthread_mutex_t lock;   /* single global lock (baseline) */
} hash_table_t;

typedef struct {
  int thread_id;
  long ops_completed;
  unsigned int rng_state;
} thread_state_t;

