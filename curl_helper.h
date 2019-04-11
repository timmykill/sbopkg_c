#ifndef CURL_HELPER_H_
#define CURL_HELPER_H_

typedef struct {
  char *memory;
  size_t size;
} MemoryStruct;

MemoryStruct* curl_from_url(char* s);

#endif
