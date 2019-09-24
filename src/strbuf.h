#ifndef STRBUF_H__
#define STRBUF_H__
#pragma once

#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STRBUF_VERSION_MAJOR 1
#define STRBUF_VERSION_MINOR 0
#define STRBUF_VERSION_PATCH 0

#if (defined(__GNUC__) && __GNUC__ >= 3) || defined(__clang__)
#  define STRBUF_ATTRIBUTE(x) __attribute__(x)
#else
#  define STRBUF_ATTRIBUTE(x) /* nothing */
#endif

#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
  #ifdef WIN_EXPORT
    // Exporting...
    #ifdef __GNUC__
      #define STRBUF_EXPORT __attribute__ ((dllexport))
    #else
      #define STRBUF_EXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define STRBUF_EXPORT __attribute__ ((dllimport))
    #else
      #define STRBUF_EXPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define STRBUF_PRIVATE
#else
  #if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
    #define STRBUF_EXPORT __attribute__ ((visibility ("default")))
    #define STRBUF_PRIVATE __attribute__ ((visibility ("hidden")))
  #else
    #define STRBUF_EXPORT
    #define STRBUF_PRIVATE
  #endif
#endif

/**
 * @brief String buffer data structure.
 * 
 */
typedef struct strbuf_s {
   size_t  capacity; ///< capacity of of buffer
   size_t  used;     ///< number of bytes used
   char   *data;     ///< buffer. might be NULL if capacity is 0
} strbuf_t;

#define STRBUF_INIT_CAPACITY 256

/**
 * @brief Create a new string buffer.
 *
 * This function creates a new string buffer and allocates a buffer
 * of the size STRBUF_INIT_CAPACITY. If memory allocation fails the data
 * field will be NULL and errno will be set accordingly.
 *
 * @return a new string buffer
 */
STRBUF_EXPORT strbuf_t strbuf_new();

/**
 * @brief Create a new string buffer with given initial capacity.
 *
 * This function creates a new string buffer and allocates a buffer
 * of the size capacity. If memory allocation fails the data
 * field will be NULL and errno will be set accordingly.
 *
 * @param capacity the size of the buffer to allocate
 * @return a new string buffer
 */
STRBUF_EXPORT strbuf_t strbuf_with_capacity(size_t capacity);

/**
 * @brief Create a new string buffer from a given string.
 * 
 * The new string buffer will have a buffer big enough to hold the given string
 * plus a terminating NUL byte. If memory allocation fails the data
 * field will be NULL and errno will be set accordingly.
 * 
 * @param str the initial string to fill the buffer with
 * @return a new string buffer
 */
STRBUF_EXPORT strbuf_t strbuf_from_str(const char *str);

/**
 * @brief Clone the given string buffer.
 *
 * Create a new string buffer as a copy from buf. If memory allocation fails
 * the data field will be NULL and errno will be set accordingly.
 *
 * @param buf a string buffer
 * @return a new string buffer
 */
STRBUF_EXPORT strbuf_t strbuf_clone(const strbuf_t *buf);

/**
 * @brief Set the used field to 0.
 * 
 * @param buf 
 */
STRBUF_EXPORT void strbuf_clear(strbuf_t *buf);

/**
 * @brief Shrink buffer to used + 1.
 * 
 * @param buf 
 * @return 0 on success, ERANGE if used == SIZE_MAX, ENOMEM if realloc() failed
 */
STRBUF_EXPORT int strbuf_shrink(strbuf_t *buf);

STRBUF_EXPORT int strbuf_truncate(strbuf_t *buf, size_t size);

STRBUF_EXPORT int strbuf_ensure_capacity(strbuf_t *buf, size_t capacity);
STRBUF_EXPORT int strbuf_fill(strbuf_t *buf, size_t count, char ch);
STRBUF_EXPORT int strbuf_append_char(strbuf_t *buf, char ch);
STRBUF_EXPORT int strbuf_append(strbuf_t *buf, const char *str);
STRBUF_EXPORT int strbuf_append_slice(strbuf_t *buf, const char *str, size_t size);
STRBUF_EXPORT int strbuf_printf(strbuf_t *buf, const char *format, ...) STRBUF_ATTRIBUTE(( format( printf, 2, 3 ) ));
STRBUF_EXPORT int strbuf_vprintf(strbuf_t *buf, const char *format, va_list ap) STRBUF_ATTRIBUTE(( format( printf, 2, 0 ) ));
STRBUF_EXPORT const char *strbuf_as_str(const strbuf_t *buf);
STRBUF_EXPORT char *strbuf_to_str(const strbuf_t *buf);
STRBUF_EXPORT char *strbuf_into_str(strbuf_t *buf);

STRBUF_EXPORT void strbuf_free(strbuf_t *buf);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* STRBUF_H__ */
