#ifndef SSTRBUF_H__
#define SSTRBUF_H__
#pragma once

#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SSTRBUF_VERSION_MAJOR 1
#define SSTRBUF_VERSION_MINOR 0
#define SSTRBUF_VERSION_PATCH 0

#if (defined(__GNUC__) && __GNUC__ >= 3) || defined(__clang__)
#  define SSTRBUF_ATTRIBUTE(x) __attribute__(x)
#else
#  define SSTRBUF_ATTRIBUTE(x) /* nothing */
#endif

#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
  #ifdef WIN_EXPORT
    // Exporting...
    #ifdef __GNUC__
      #define SSTRBUF_EXPORT __attribute__ ((dllexport))
    #else
      #define SSTRBUF_EXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define SSTRBUF_EXPORT __attribute__ ((dllimport))
    #else
      #define SSTRBUF_EXPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define SSTRBUF_PRIVATE
#else
  #if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
    #define SSTRBUF_EXPORT __attribute__ ((visibility ("default")))
    #define SSTRBUF_PRIVATE __attribute__ ((visibility ("hidden")))
  #else
    #define SSTRBUF_EXPORT
    #define SSTRBUF_PRIVATE
  #endif
#endif

/**
 * @brief 
 * 
 */
typedef struct sstrbuf_s {
   size_t  capacity;
   size_t  used;
   char   *data;
} sstrbuf_t;

SSTRBUF_EXPORT sstrbuf_t sstrbuf_new(char *data, size_t capacity);

#define SSTRBUF_NEW(DATA) sstrbuf_new((DATA), sizeof (DATA))
#define SSTRBUF_DECL(NAME, SIZE) \
	char _ ## NAME ## _data[SIZE]; \
	sstrbuf_t NAME = sstrbuf_new(_ ## NAME ## _data, (SIZE));

SSTRBUF_EXPORT void sstrbuf_clear(sstrbuf_t *buf);
SSTRBUF_EXPORT int sstrbuf_truncate(sstrbuf_t *buf, size_t size);
SSTRBUF_EXPORT int sstrbuf_fill(sstrbuf_t *buf, size_t count, char ch);
SSTRBUF_EXPORT int sstrbuf_append_char(sstrbuf_t *buf, char ch);
SSTRBUF_EXPORT int sstrbuf_append(sstrbuf_t *buf, const char *str);
SSTRBUF_EXPORT int sstrbuf_append_slice(sstrbuf_t *buf, const char *str, size_t size);
SSTRBUF_EXPORT int sstrbuf_printf(sstrbuf_t *buf, const char *format, ...) SSTRBUF_ATTRIBUTE(( format( printf, 2, 3 ) ));
SSTRBUF_EXPORT int sstrbuf_vprintf(sstrbuf_t *buf, const char *format, va_list ap) SSTRBUF_ATTRIBUTE(( format( printf, 2, 0 ) ));
SSTRBUF_EXPORT char *sstrbuf_as_str(sstrbuf_t *buf);
SSTRBUF_EXPORT char *sstrbuf_to_str(const sstrbuf_t *buf);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* SSTRBUF_H__ */
