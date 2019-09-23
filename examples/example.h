#ifndef STRBUF_EXAMPLE_H__
#define STRBUF_EXAMPLE_H__

#include <string.h>
#include <stdio.h>

#define STR(X) #X
#define TRY(EXPR) { const int errnum = (EXPR); if (errnum != 0) { fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, STR(EXPR), strerror(errnum)); exit(1); } }

#endif