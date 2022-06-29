#include <stdint.h>
#include <string.h>

char *strncpy(char * dest, const char * src, size_t n){
    if (n != 0) {
	char *d = dest;
	const char *s = src;
	do {
	  	if ((*d++ = *s++) == '\0') {
				while (--n != 0)
					*d++ = '\0';
				break;
			}
		} while (--n != 0);
	}
	return (dest);
}
