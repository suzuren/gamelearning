#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "atomic.h"

#include "malloc_hook.h"

#define MEMORY_ALLOCTAG 0x20140605
#define MEMORY_FREETAG 0x0badf00d

static size_t _used_memory = 0;
static size_t _memory_block = 0;

struct mem_data {
	uint32_t handle;
	ssize_t allocated;
};

struct mem_cookie {
	uint32_t handle;
#ifdef MEMORY_CHECK
	uint32_t dogtag;
#endif
};

#define SLOT_SIZE 0x10000
#define PREFIX_SIZE sizeof(struct mem_cookie)

static struct mem_data mem_stats[SLOT_SIZE];


#ifndef NOUSE_JEMALLOC

#include "/root/code/jemalloc/include/jemalloc/jemalloc.h"

// for skynet_lalloc use
#define raw_realloc je_realloc
#define raw_free je_free

static ssize_t*
get_allocated_field(uint32_t handle) {
	int h = (int)(handle & (SLOT_SIZE - 1));
	struct mem_data *data = &mem_stats[h];
	uint32_t old_handle = data->handle;
	ssize_t old_alloc = data->allocated;
	if(old_handle == 0 || old_alloc <= 0) {
		// data->allocated may less than zero, because it may not count at start.
		if(!ATOM_CAS(&data->handle, old_handle, handle)) {
			return 0;
		}
		if (old_alloc < 0) {
			ATOM_CAS(&data->allocated, old_alloc, 0);
		}
	}
	if(data->handle != handle) {
		return 0;
	}
	return &data->allocated;
}

inline static void 
update_xmalloc_stat_alloc(uint32_t handle, size_t __n) {
	ATOM_ADD(&_used_memory, __n);
	ATOM_INC(&_memory_block); 
	ssize_t* allocated = get_allocated_field(handle);
	if(allocated) {
		ATOM_ADD(allocated, __n);
	}
}

inline static void
update_xmalloc_stat_free(uint32_t handle, size_t __n) {
	ATOM_SUB(&_used_memory, __n);
	ATOM_DEC(&_memory_block);
	ssize_t* allocated = get_allocated_field(handle);
	if(allocated) {
		ATOM_SUB(allocated, __n);
	}
}



static void malloc_oom(size_t size) {
	fprintf(stderr, "xmalloc: Out of memory trying to allocate %zu bytes\n",
		size);
	fflush(stderr);
	abort();
}


// hook : malloc, realloc, free, calloc

void *
skynet_malloc(size_t size) {
	void* ptr = je_malloc(size + PREFIX_SIZE);
	if(!ptr) malloc_oom(size);
	return fill_prefix(ptr);
}

void *
skynet_realloc(void *ptr, size_t size) {
	if (ptr == NULL) return skynet_malloc(size);

	void* rawptr = clean_prefix(ptr);
	void *newptr = je_realloc(rawptr, size+PREFIX_SIZE);
	if(!newptr) malloc_oom(size);
	return fill_prefix(newptr);
}

void
skynet_free(void *ptr) {
	if (ptr == NULL) return;
	void* rawptr = clean_prefix(ptr);
	je_free(rawptr);
}

void *
skynet_calloc(size_t nmemb,size_t size) {
	void* ptr = je_calloc(nmemb + ((PREFIX_SIZE+size-1)/size), size );
	if(!ptr) malloc_oom(size);
	return fill_prefix(ptr);
}

void *
skynet_memalign(size_t alignment, size_t size) {
	void* ptr = je_memalign(alignment, size + PREFIX_SIZE);
	if(!ptr) malloc_oom(size);
	return fill_prefix(ptr);
}

#else

// for skynet_lalloc use
#define raw_realloc realloc
#define raw_free free

void 
memory_info_dump(void) {
	skynet_error(NULL, "No jemalloc");
}

size_t 
mallctl_int64(const char* name, size_t* newval) {
	skynet_error(NULL, "No jemalloc : mallctl_int64 %s.", name);
	return 0;
}

int 
mallctl_opt(const char* name, int* newval) {
	skynet_error(NULL, "No jemalloc : mallctl_opt %s.", name);
	return 0;
}

#endif

size_t
malloc_used_memory(void) {
	return _used_memory;
}

size_t
malloc_memory_block(void) {
	return _memory_block;
}

void
dump_c_mem() {
	int i;
	size_t total = 0;
	printf("dump all service mem:");
	for(i=0; i<SLOT_SIZE; i++) {
		struct mem_data* data = &mem_stats[i];
		if(data->handle != 0 && data->allocated != 0) {
			total += data->allocated;
			printf(":%08x -> %zdkb %db", data->handle, data->allocated >> 10, (int)(data->allocated % 1024));
		}
	}
	printf(NULL, "+total: %zdkb",total >> 10);
}

char *
skynet_strdup(const char *str) {
	size_t sz = strlen(str);
	char * ret = skynet_malloc(sz+1);
	memcpy(ret, str, sz+1);
	return ret;
}

void * 
skynet_lalloc(void *ptr, size_t osize, size_t nsize) {
	if (nsize == 0) {
		raw_free(ptr);
		return NULL;
	} else {
		return raw_realloc(ptr, nsize);
	}
}


