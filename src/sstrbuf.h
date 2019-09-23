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

typedef struct sstrbuf_s {
   size_t  capacity;
   size_t  used;
   char   *data;
} sstrbuf_t;

sstrbuf_t sstrbuf_new(char *data, size_t capacity);
#define SSTRBUF_NEW(DATA) strbuf_new((DATA), sizeof (DATA))
#define SSTRBUF_DECL(NAME, SIZE) \
	char _ ## NAME ## _data[SIZE]; \
	strbuf_t NAME = strbuf_new(_ ## NAME ## _data, (SIZE));

void sstrbuf_clear(sstrbuf_t *buf);
int sstrbuf_resize(sstrbuf_t *buf, size_t size);
int sstrbuf_fill(sstrbuf_t *buf, size_t count, char ch);
int sstrbuf_append_char(sstrbuf_t *buf, char ch);
int sstrbuf_append(sstrbuf_t *buf, const char *str);
int sstrbuf_append_slice(sstrbuf_t *buf, const char *str, size_t count);
int sstrbuf_printf(sstrbuf_t *buf, const char *format, ...) SSTRBUF_ATTRIBUTE(( format( printf, 2, 3 ) ));
int sstrbuf_vprintf(sstrbuf_t *buf, const char *format, va_list ap) SSTRBUF_ATTRIBUTE(( format( printf, 2, 0 ) ));
char *sstrbuf_as_str(sstrbuf_t *buf);
char *sstrbuf_to_str(const sstrbuf_t *buf);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* SSTRBUF_H__ */
