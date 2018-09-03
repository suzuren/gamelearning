
//--------------------------------------------------------------------------

#include "databuffer.h"
#include "hashid.h"

#include <stdio.h>

//--------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    printf("test databuffer hashid .\n");

	//--------------------------------------------------------------------------

	struct hashid hash;
	int max = 16;
	int id;

	hashid_init(&hash, max);
	id = hashid_insert(&hash,3);
	printf("hashid_insert - id:%d\n", id);
	id = hashid_insert(&hash, 4);
	printf("hashid_insert - id:%d\n", id);
	id = hashid_insert(&hash, 5);
	printf("hashid_insert - id:%d\n", id);
	id = hashid_insert(&hash, 5);
	printf("hashid_insert - id:%d\n", id);

	id = hashid_remove(&hash, 5);
	printf("hashid_remove - id:%d\n", id);
	id = hashid_remove(&hash, 5);
	printf("hashid_remove - id:%d\n", id);
	id = hashid_remove(&hash, 6);
	printf("hashid_remove - id:%d\n", id);

	id = hashid_lookup(&hash, 5);
	printf("hashid_lookup - id:%d\n", id);
	id = hashid_lookup(&hash, 3);
	printf("hashid_lookup - id:%d\n", id);

	int full = hashid_full(&hash);
	printf("hashid_full - full:%d\n", full);

	hashid_clear(&hash);

	//--------------------------------------------------------------------------

	//struct databuffer buffer;
	//struct messagepool mp;

	//int size = 1024;
	//
	//int header_size = 128;
	//int rhz = databuffer_readheader(&buffer, &mp, header_size);
	//printf("databuffer_readheader - rhz:%d\n", rhz);

	//void * temp = skynet_malloc(size);
	//int rz = databuffer_read(&buffer, &mp, temp, size);
	//printf("databuffer_readheader - rz:%d\n", rz);

	//void * data = skynet_malloc(size);
	//databuffer_push(&buffer, &mp, data, size);
	//skynet_free(data);
	//printf("databuffer_push - size:%d\n", size);


	//databuffer_reset(&buffer);

	//databuffer_clear(&buffer, &mp);
	//messagepool_free(&mp);


	//--------------------------------------------------------------------------

     return 0;
}


//--------------------------------------------------------------------------