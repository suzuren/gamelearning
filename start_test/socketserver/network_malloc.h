#ifndef skynet_malloc_h
#define skynet_malloc_h

#include <stddef.h>
#include <stdlib.h>
#include <string.h> 
#include <stdint.h>

#define network_malloc malloc
#define network_free free

//void * skynet_realloc(void *ptr, size_t size)
//{
//	if (ptr == NULL) return skynet_malloc(size);
//	void* rawptr = ptr;
//	void *newptr = realloc(rawptr, size);
//	return newptr;
//}

//void * skynet_copymem(void *des_ptr, size_t des_size, void *src_buff, size_t src_size)
//{
//	void *newptr = skynet_realloc(des_ptr, src_size);
//	memcpy(newptr + des_size, src_buff, src_size);
//	return newptr;
//	return NULL;
//}
//

#endif
