

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<dlfcn.h>
#include "/root/code/jemalloc/include/jemalloc/jemalloc.h"

void malloc_oom(size_t size)
{
	fprintf(stderr, "xmalloc: Out of memory trying to allocate %zu bytes\n",size);
	fflush(stderr);
	abort();
}

void * skynet_malloc(size_t size)
{
	void* ptr = je_malloc(size);
	if (!ptr) malloc_oom(size);
	return ptr;
}

void * skynet_realloc(void *ptr, size_t size)
{
	if (ptr == NULL) return skynet_malloc(size);
	void* rawptr = ptr;
	void *newptr = je_realloc(rawptr, size);
	if (!newptr) malloc_oom(size);
	return newptr;
}

void * skynet_calloc(size_t nmemb, size_t size)
{
	void* ptr = je_calloc(nmemb, size);
	if (!ptr) malloc_oom(nmemb * size);
	return ptr;
}

void * skynet_memalign(size_t alignment, size_t size)
{
	void* ptr = je_memalign(alignment, size);
	if (!ptr) malloc_oom(size);
	return ptr;
}


void skynet_free(void *ptr)
{
	if (ptr == NULL) return;
	je_free(ptr);
}

int main(int argc, const char** argv)
{
	printf("hello test je_malloc!\n");

	void* ptr = je_malloc(16384);
	if (ptr == NULL)
	{
		malloc_oom(16384);
	}
	size_t usea_size = je_malloc_usable_size(ptr);
	printf("usea_size:%lu,   ptr:%p\n", usea_size, ptr);
	ptr = je_realloc(ptr, 65536);
	usea_size = je_malloc_usable_size(ptr);
	printf("usea_size:%lu,   ptr:%p\n", usea_size, ptr);
	je_free(ptr);
	
	void * malptr = skynet_malloc(65536);
	usea_size = je_malloc_usable_size(malptr);
	printf("usea_size:%lu,malptr:%p\n", usea_size, malptr);
	je_free(malptr);

	//void * reaptr = skynet_realloc(NULL,65536);
	void * tmpptr = skynet_malloc(65536);
	void * reaptr = skynet_realloc(tmpptr, 65536);
	usea_size = je_malloc_usable_size(reaptr);
	printf("usea_size:%lu,reaptr:%p\n", usea_size, reaptr);
	je_free(reaptr);

	void * calptr = skynet_calloc(128, 512);
	usea_size = je_malloc_usable_size(calptr);
	printf("usea_size:%lu,calptr:%p\n", usea_size, calptr);
	je_free(calptr);


	void * memptr = skynet_memalign(64, 65536);
	usea_size = je_malloc_usable_size(memptr);
	printf("usea_size:%lu,memptr:%p\n", usea_size, memptr);
	je_free(memptr);

	//for (size_t i = 0; i < 10; i++)
	//{
	//	// Leak some memory.
	//	je_malloc(i * 100);
	//}
	//// Dump allocator statistics to stderr.
	//je_malloc_stats_print(NULL, NULL, NULL);

	return 0;
}
