#include "strbuf.h"
#include "example.h"

#include <stdlib.h>

int main() {
	strbuf_t buf = strbuf_new();
	TRY(strbuf_append(&buf, "Hell"));
	TRY(strbuf_append_slice(&buf, "o xxx", 2));
	TRY(strbuf_printf(&buf, "%s! %02d", "world", 42));
	
	char *str = strbuf_to_str(&buf);
	puts(str);
	free(str);

	strbuf_t buf2 = strbuf_clone(&buf);
	strbuf_free(&buf);
	TRY(strbuf_append_char(&buf2, '!'));

	str = strbuf_into_str(&buf2);
	puts(str);
	free(str);

	return 0;
}
