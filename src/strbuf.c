#include "strbuf.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

strbuf_t strbuf_with_capacity(size_t capacity) {
	strbuf_t buf = {
		.capacity = 0,
		.used = 0,
		.data = NULL
	};

	int errnum = strbuf_ensure_capacity(&buf, capacity);
	if (errnum != 0) {
		errno = errnum;
	}

	return buf;
}

strbuf_t strbuf_new() {
	return strbuf_with_capacity(STRBUF_INIT_CAPACITY);
}

strbuf_t strbuf_from_str(const char *str) {
	size_t len = strlen(str);
	if (len >= SIZE_MAX) {
		errno = ERANGE;
		return (strbuf_t){ .capacity = 0, .used = 0, .data = NULL };
	}
	strbuf_t buf = strbuf_with_capacity(len + 1);
	if (buf.data) {
		memcpy(buf.data, str, len);
		buf.used = len;
	}
	return buf;
}

strbuf_t strbuf_clone(const strbuf_t *other) {
	strbuf_t buf = {
		.capacity = 0,
		.used = 0,
		.data = NULL
	};

	if (other->capacity > 0) {
		buf.data = malloc(other->capacity);
		if (!buf.data) {
			return buf;
		}
		buf.capacity = other->capacity;
		memcpy(buf.data, other->data, other->used);
		buf.used = other->used;
	}

	return buf;
}

void strbuf_clear(strbuf_t *buf) {
	buf->used = 0;
}

int strbuf_shrink(strbuf_t *buf) {
	if (buf->used == SIZE_MAX) {
		return ERANGE;
	}

	size_t capacity = buf->used + 1;
	if (capacity < buf->capacity) {
		char *newdata = realloc(buf->data, capacity);
		if (!newdata) {
			return errno;
		}
		buf->data = newdata;
	}

	return 0;
}

int strbuf_fill(strbuf_t *buf, size_t count, char ch) {
	if (count >= SIZE_MAX) {
		return ERANGE;
	}
	if (buf->used >= SIZE_MAX - count - 1) {
		return ERANGE;
	}
	int errnum = strbuf_ensure_capacity(buf, buf->used + count + 1);
	if (errnum != 0) {
		return errnum;
	}
	memset(buf->data + buf->used, ch, count);
	buf->used += count;
	return 0;
}

int strbuf_truncate(strbuf_t *buf, size_t size) {
	if (size > buf->used) {
		return ERANGE;
	}

	buf->used = size;
	return 0;
}

int strbuf_ensure_capacity(strbuf_t *buf, size_t capacity) {
	if (capacity < buf->capacity) {
		return 0;
	}
	if (capacity == SIZE_MAX) {
		return ERANGE;
	}
	// over-allocate: round to the next power of 2
	size_t big_capacity = capacity;

	big_capacity --;
#if SIZE_MAX >= UINT8_MAX
	big_capacity |= big_capacity >> 1;
	big_capacity |= big_capacity >> 2;
	big_capacity |= big_capacity >> 4;
#else
#	error architecture not supported
#endif

#if SIZE_MAX >= UINT16_MAX
	big_capacity |= big_capacity >> 8;
#endif

#if SIZE_MAX >= UINT32_MAX
	big_capacity |= big_capacity >> 16;
#endif

#if SIZE_MAX >= UINT64_MAX
	big_capacity |= big_capacity >> 32;
#endif

#if defined(UINT128_MAX)
#	if SIZE_MAX == UINT128_MAX
	big_capacity |= big_capacity >> 64;
#	else
#		error architecture not supported
#	endif
#elif SIZE_MAX > UINT64_MAX
#	error architecture not supported
#endif
	big_capacity ++;

	if (big_capacity < capacity) {
		return ERANGE;
	}

	char *newdata = realloc(buf->data, big_capacity);
	if (!newdata) {
		int errnum = errno;
		if (errnum == ENOMEM) {
			big_capacity = capacity + 1;
			newdata = realloc(buf->data, big_capacity);
			if (!newdata) {
				return errno;
			}
		} else {
			return errnum;
		}
	}
	buf->data = newdata;
	buf->capacity = big_capacity;
	return 0;
}

int strbuf_append(strbuf_t *buf, const char *str) {
	size_t len = strlen(str);
	if (len >= SIZE_MAX) {
		return ERANGE;
	}
	if (buf->used >= SIZE_MAX - len - 1) {
		return ERANGE;
	}
	int errnum = strbuf_ensure_capacity(buf, buf->used + len + 1);
	if (errnum != 0) {
		return errnum;
	}
	memcpy(buf->data + buf->used, str, len);
	buf->used += len;
	return 0;
}

int strbuf_append_char(strbuf_t *buf, char ch) {
	if (buf->used >= SIZE_MAX - 2) {
		return ERANGE;
	}
	int errnum = strbuf_ensure_capacity(buf, buf->used + 2);
	if (errnum != 0) {
		return errnum;
	}
	buf->data[buf->used] = ch;
	buf->used ++;
	return 0;
}

int strbuf_append_slice(strbuf_t *buf, const char *str, size_t size) {
	size_t len = strnlen(str, size);
	if (len >= SIZE_MAX) {
		return ERANGE;
	}
	if (buf->used >= SIZE_MAX - len - 1) {
		return ERANGE;
	}
	int errnum = strbuf_ensure_capacity(buf, buf->used + len + 1);
	if (errnum != 0) {
		return errnum;
	}
	memcpy(buf->data + buf->used, str, len);
	buf->used += len;
	return 0;
}

int strbuf_printf(strbuf_t *buf, const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	int errnum = strbuf_vprintf(buf, format, ap);
	va_end(ap);
	return errnum;
}

int strbuf_vprintf(strbuf_t *buf, const char *format, va_list ap) {
	size_t remaining_capacity = buf->capacity - buf->used;
	int size = vsnprintf(buf->data + buf->used, remaining_capacity, format, ap);
	if (size < 0) {
		int errnum = errno;
		return errnum ? errnum : EINVAL;
	}
	if ((size_t)size > remaining_capacity) {
		if ((size_t)size >= SIZE_MAX || buf->used >= SIZE_MAX - size - 1) {
			return ERANGE;
		}
		int errnum = strbuf_ensure_capacity(buf, buf->used + size + 1);
		if (errnum != 0) {
			return errnum;
		}
		int size2 = vsnprintf(buf->data + buf->used, buf->capacity - buf->used, format, ap);
		assert(size2 == size);
		buf->used += size2;
	} else if ((size_t)size == remaining_capacity) {
		if (buf->capacity >= SIZE_MAX) {
			return ERANGE;
		}
		// Format was written, except the terminating NUL had no room.
		// We don't actually need that NUL right now, but we always ensure
		// that there would be enough room for it after any write to the
		// buffer. This way strbuf_as_str() won't ever need to allocate.
		return strbuf_ensure_capacity(buf, buf->capacity + 1);
	}
	buf->used += size;
	return 0;
}

char *strbuf_to_str(const strbuf_t *buf) {
	if (buf->used == SIZE_MAX) {
		errno = ERANGE;
		return NULL;
	}
	char *str = malloc(buf->used + 1);
	memcpy(str, buf->data, buf->used);
	str[buf->used] = 0;
	return str;
}

char *strbuf_into_str(strbuf_t *buf) {
	char *str;
	if (buf->used < buf->capacity) {
		buf->data[buf->used] = 0;
		// shrink data to not be wastefull
		str = realloc(buf->data, buf->used + 1);
		assert(str != NULL);
		if (!str) {
			// fallback if srink failed (which shoulding happen)
			str = buf->data;
		}
	} else if (buf->used == SIZE_MAX) {
		errno = ERANGE;
		return NULL;
	} else {
		assert(buf->used == buf->capacity);
		str = realloc(buf->data, buf->used + 1);
		if (!str) {
			return NULL;
		}
		str[buf->used] = 0;
	}

	buf->data = NULL;
	buf->capacity = 0;
	buf->used = 0;
	return str;
}

const char *strbuf_as_str(const strbuf_t *buf) {
	if (buf->used == 0) {
		// this allows null-buffers where { .capacity = 0, .used = 0, .data = NULL }
		return "";
	} else if (buf->used == buf->capacity) {
		// should not happen since we always allocate at least one more byte
		errno = EINVAL;
		assert(buf->used != buf->capacity);
		return NULL;
	} else {
		((strbuf_t*) buf)->data[buf->used] = 0;
		return buf->data;
	}
}

void strbuf_free(strbuf_t *buf) {
	free(buf->data);
	buf->data = NULL;
	buf->capacity = 0;
	buf->used = 0;
}
