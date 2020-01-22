#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

int hash(char* s)
{
	int i, ret = 0;
	for (i=0; s[i] != '\0'; i++)
		ret += s[i];
	return ret;
}

char * malloc_safe(size_t size, char* fail_string)
{
	char * tmp = malloc(size);
	if (tmp == NULL)
		die(fail_string);
	return tmp;

}


void die(char* error)
{
	perror(error);
	exit(EXIT_FAILURE);
}
