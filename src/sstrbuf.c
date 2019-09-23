#include "sstrbuf.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

sstrbuf_t sstrbuf_new(char *data, size_t capacity) {
	sstrbuf_t buf = {
		.capacity = capacity,
		.used = 0,
		.data = data
	};
	return buf;
}

void sstrbuf_clear(sstrbuf_t *buf) {
	buf->used = 0;
}

int sstrbuf_fill(sstrbuf_t *buf, size_t count, char ch) {
	if (count >= buf->capacity) {
		return ERANGE;
	}
	if (buf->used >= buf->capacity - count) {
		return ERANGE;
	}
	memset(buf->data + buf->used, ch, count);
	buf->used += count;
	return 0;
}

int sstrbuf_resize(sstrbuf_t *buf, size_t size) {
	if (size >= buf->capacity) {
		return ERANGE;
	}
	if (size <= buf->used) {
		buf->used = size;
		return 0;
	} else if (buf->used >= buf->capacity - size) {
		return ERANGE;
	} else {
		size_t oldsize = buf->used;
		memset(buf->data + oldsize, 0, size - oldsize);
		buf->used += size;
		return 0;
	}
}

int sstrbuf_append(sstrbuf_t *buf, const char *str) {
	size_t len = strlen(str);
	if (len >= buf->capacity) {
		return ERANGE;
	}
	if (buf->used >= buf->capacity - len) {
		return ERANGE;
	}
	memcpy(buf->data + buf->used, str, len);
	buf->used += len;
	return 0;
}

int sstrbuf_append_char(sstrbuf_t *buf, char ch) {
	if (buf->capacity == 0 || buf->used >= buf->capacity - 1) {
		return ERANGE;
	}
	buf->data[buf->used] = ch;
	buf->used ++;
	return 0;
}

int sstrbuf_append_slice(sstrbuf_t *buf, const char *str, size_t count) {
	size_t len = strnlen(str, count);
	if (len >= buf->capacity) {
		return ERANGE;
	}
	if (buf->used >= buf->capacity - len) {
		return ERANGE;
	}
	memcpy(buf->data + buf->used, str, len);
	buf->used += len;
	return 0;
}

int sstrbuf_printf(sstrbuf_t *buf, const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	int errnum = sstrbuf_vpritnf(buf, format, ap);
	va_end(ap);
	return errnum;
}

int sstrbuf_vprintf(sstrbuf_t *buf, const char *format, va_list ap) {
	size_t remaining_capacity = buf->capacity - buf->used;
	int size = vsnprintf(buf->data + buf->used, remaining_capacity, format, ap);
	if (size < 0) {
		int errnum = errno;
		return errnum ? errnum : EINVAL;
	}
	if (size >= remaining_capacity) {
		return ERANGE;
	}
	return 0;
}

char *sstrbuf_to_str(const sstrbuf_t *buf) {
	if (buf->used == SIZE_MAX) {
		errno = ERANGE;
		return NULL;
	}
	char *str = malloc(buf->used + 1);
	memcpy(str, buf->data, buf->used);
	str[buf->used] = 0;
	return str;
}

char *sstrbuf_as_str(sstrbuf_t *buf) {
	if (buf->used == buf->capacity) {
		// should not happen since we always leave one byte space
		errno = EINVAL;
		assert(buf->used != buf->capacity);
		return NULL;
	} else {
		buf->data[buf->used] = 0;
		return buf->data;
	}
}
