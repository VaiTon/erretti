#ifndef UTILS_H
#define UTILS_H

#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#ifndef NDEBUG
#define RT_DEBUG(...)                                                                              \
  do {                                                                                             \
    fprintf(stderr, "DEBUG [%s:%d]: ", __func__, __LINE__);                                        \
    fprintf(stderr, __VA_ARGS__);                                                                  \
    fprintf(stderr, "\n");                                                                         \
  } while (0)
#else
#define RT_DEBUG(...)
#endif

#define RT_ERROR(...)                                                                              \
  do {                                                                                             \
    fprintf(stderr, "ERROR [%s:%d]: ", __func__, __LINE__);                                        \
    fprintf(stderr, __VA_ARGS__);                                                                  \
    fprintf(stderr, "\n");                                                                         \
  } while (0)

#define RT_INFO(...)                                                                               \
  do {                                                                                             \
    fprintf(stderr, "INFO [%s:%d]: ", __func__, __LINE__);                                         \
    fprintf(stderr, __VA_ARGS__);                                                                  \
    fprintf(stderr, "\n");                                                                         \
  } while (0)

#define RT_WARN(...)                                                                               \
  do {                                                                                             \
    fprintf(stderr, "WARNING [%s:%d]: ", __func__, __LINE__);                                      \
    fprintf(stderr, __VA_ARGS__);                                                                  \
    fprintf(stderr, "\n");                                                                         \
  } while (0)

#define RT_FATAL(...)                                                                              \
  do {                                                                                             \
    fprintf(stderr, "!!! FATAL [%s:%d]: ", __func__, __LINE__);                                    \
    fprintf(stderr, __VA_ARGS__);                                                                  \
    fprintf(stderr, " !!!\n");                                                                     \
    exit(EXIT_FAILURE);                                                                            \
  } while (0)

#ifndef NDEBUG
#define RT_ASSERT(condition, ...)                                                                  \
  do {                                                                                             \
    if (!(condition)) {                                                                            \
      fprintf(stderr, "ASSERTION FAILED [%s:%d]: ", __func__, __LINE__);                           \
      fprintf(stderr, __VA_ARGS__);                                                                \
      fprintf(stderr, "\n");                                                                       \
      exit(EXIT_FAILURE);                                                                          \
    }                                                                                              \
  } while (0)
#else
#define RT_ASSERT(condition, ...)
#endif

#define RT_CHECK_MALLOC(ptr)                                                                       \
  do {                                                                                             \
    if ((ptr) == NULL) {                                                                           \
      RT_FATAL("Memory allocation failed");                                                        \
    }                                                                                              \
  } while (0)

// Genera un numero casuale tra 0 e 1
static inline double random_double() {
  // Thread-local RNG state
  static thread_local unsigned int rng_state;
  return (double)rand_r(&rng_state) / (double)RAND_MAX;
}

// Genera un numero casuale tra 0 e 1
static inline double random_double_range(const double min, const double max) {
  return min + (max - min) * random_double();
}

static inline double degrees_to_radians(double degrees) { return degrees * M_PI / 180.0; }

#endif
