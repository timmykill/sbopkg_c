#ifndef _UTILS_H_
#define _UTILS_H_

int hash(char* s);
char * malloc_safe(size_t size, char* fail_string);
void die(char* error);

#endif
