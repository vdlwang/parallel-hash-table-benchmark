#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "structs.h"
#include <stdio.h>

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
