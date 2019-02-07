#include <stdlib.h>
#include <s_heap.h>
#include <string.h>

extern heap_t g_my_heap;

void *malloc(size_t size)
{
  return s_alloc(size, &g_my_heap);
}

void free(void *ptr)
{
  s_free(ptr, &g_my_heap);
}

void *calloc(size_t nmemb, size_t size)
{
  uint8_t *ptr = malloc(nmemb * size);

  for (int i = 0; i < size; i++)
    memset(ptr + nmemb * i, 0, nmemb);
}

void *realloc(void *ptr, size_t size)
{
}

void *reallocarray(void *ptr, size_t nmemb, size_t size)
{
}