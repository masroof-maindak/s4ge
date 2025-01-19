#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/utils.h"

/**
 * @details if adding `add` bytes to `buf`, (whose maximum capacity is
 * `capacity` and currently has `idx` bytes written), would overflow it, then
 * double `buf`. `capacity` is updated in this case.
 *
 * @return NULL on failure, buffer on success
 */
char *double_if_Of(char *buf, size_t idx, size_t add, size_t *capacity) {
	char *tmp = NULL;

	if (idx + add > *capacity) {
		*capacity *= 2;
		if ((tmp = realloc(buf, *capacity)) == NULL) {
			perror("realloc() in double_if_Of()");
			free(buf);
			return NULL;
		}
		buf = tmp;
	}

	return buf;
}

char *copy_string(const char *str) {
	if (str == NULL) {
		fprintf(stderr, "copy_string(): receieved null input\n");
		return NULL;
	}

	size_t size = strlen(str);
	char *copy	= malloc(size + 1);
	if (copy == NULL) {
		perror("malloc() in copy_string()");
		return NULL;
	}

	memcpy(copy, str, size);
	copy[size] = '\0';
	return copy;
}
