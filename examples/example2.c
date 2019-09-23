#include "sstrbuf.h"
#include "example.h"

#include <stdlib.h>

int main() {
	SSTRBUF_DECL(buf, BUFSIZ);
	TRY(sstrbuf_append(&buf, "Hell"));
	TRY(sstrbuf_append_slice(&buf, "o xxx", 2));
	TRY(sstrbuf_printf(&buf, "%s! %02d", "world", 42));
	
	char *str = sstrbuf_to_str(&buf);
	puts(str);
	free(str);

	TRY(sstrbuf_append_char(&buf, '!'));

	str = sstrbuf_as_str(&buf);
	puts(str);

	return 0;
}
