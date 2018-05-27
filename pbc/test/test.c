#include "pbc.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

//#define COUNT 1000000
#define COUNT 10

#include "readfile.h"

static void
test(struct pbc_env *env) {
	int i;
	for(i=0; i<COUNT; i++)
	{
			//写数据 保存在
			struct pbc_wmessage* w_msg = pbc_wmessage_new(env, "at");
			struct pbc_rmessage* r_msg = NULL;
			struct pbc_slice sl;
			char buffer[1024];
			sl.buffer = buffer, sl.len = 1024;
			pbc_wmessage_integer(w_msg, "aa", 123, 0);
			pbc_wmessage_integer(w_msg, "bb", 456, 0);
			pbc_wmessage_string(w_msg, "cc", "test string!", -1);


			pbc_wmessage_buffer(w_msg, &sl);
			
			r_msg = pbc_rmessage_new(env, "at", &sl);
			uint32_t aa = pbc_rmessage_integer(r_msg, "aa",0, NULL);
			//uint32_t pbc_rmessage_integer(struct pbc_rmessage *, const char *key, int index, uint32_t *hi);

			uint32_t bb = pbc_rmessage_integer(r_msg, "bb",1, NULL);
			const char * cc = pbc_rmessage_string(r_msg, "cc", 3, NULL);
			//const char * pbc_rmessage_string(struct pbc_rmessage *, const char *key, int index, int *sz);


			pbc_rmessage_delete(r_msg);
			pbc_wmessage_delete(w_msg);

			printf("aa:%d,bb:%d,cc:%s\n",aa,bb,cc);

	} 
}

int
main() {
	struct pbc_env * env = pbc_new();
	struct pbc_slice slice;
	read_file("test.pb", &slice);
	int ret = pbc_register(env, &slice);
	assert(ret == 0);
	test(env);
	pbc_delete(env);

	return 0;
}
