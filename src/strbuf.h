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

typedef struct strbuf_s {
   size_t  capacity;
   size_t  used;
   char   *data;
} strbuf_t;

#define STRBUF_INIT_CAPACITY 256

strbuf_t strbuf_new();
strbuf_t strbuf_with_capacity(size_t capacity);
strbuf_t strbuf_from_str(const char *str);
strbuf_t strbuf_clone(const strbuf_t *buf);

void strbuf_clear(strbuf_t *buf);
int strbuf_resize(strbuf_t *buf, size_t size);
int strbuf_ensure_capacity(strbuf_t *buf, size_t capacity);
int strbuf_fill(strbuf_t *buf, size_t count, char ch);
int strbuf_append_char(strbuf_t *buf, char ch);
int strbuf_append(strbuf_t *buf, const char *str);
int strbuf_append_slice(strbuf_t *buf, const char *str, size_t count);
int strbuf_printf(strbuf_t *buf, const char *format, ...) STRBUF_ATTRIBUTE(( format( printf, 2, 3 ) ));
int strbuf_vprintf(strbuf_t *buf, const char *format, va_list ap) STRBUF_ATTRIBUTE(( format( printf, 2, 0 ) ));
const char *strbuf_as_str(const strbuf_t *buf);
char *strbuf_to_str(const strbuf_t *buf);
char *strbuf_into_str(strbuf_t *buf);

void strbuf_free(strbuf_t *buf);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* STRBUF_H__ */
