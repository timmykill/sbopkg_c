#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "curl_helper.h"

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  MemoryStruct *mem = (MemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  return realsize;
}

static size_t write_to_file(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

MemoryStruct* curl_from_url(char* s)
{
  CURL *curl_handle;
  CURLcode res;
  MemoryStruct* chunk_p;
	chunk_p = (MemoryStruct*) malloc(sizeof(MemoryStruct));
  chunk_p->memory = malloc(1);  
  chunk_p->size = 0;
  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, s);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)chunk_p);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  res = curl_easy_perform(curl_handle);
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
		chunk_p = NULL;
	}
  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();
  return chunk_p;
}

int curl_to_file(char* url, char* dir)
{
	CURL *curl_handle;
  FILE *pagefile;
	printf("Getting: %s\n", url);
  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_to_file);
  pagefile = fopen(dir, "wb");
  if(pagefile != NULL) {
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
    curl_easy_perform(curl_handle);
    fclose(pagefile);
  } else {
		printf("Could not write %s\n", dir);
  }
	curl_easy_cleanup(curl_handle);
  curl_global_cleanup();
  return 0;
} 
