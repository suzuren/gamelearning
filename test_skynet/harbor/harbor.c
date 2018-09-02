
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <signal.h>
#include <assert.h>


//--------------------------------------------------------------------------
static unsigned int HARBOR = ~0;

// reserve high 8 bits for remote id
#define HANDLE_MASK 0xffffff
#define HANDLE_REMOTE_SHIFT 24

//--------------------------------------------------------------------------

void
skynet_harbor_init(int harbor) {
	HARBOR = (unsigned int)harbor << HANDLE_REMOTE_SHIFT;
}

//--------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    printf("harbor 1 - HARBOR:%d\n",HARBOR);
    printf("harbor 2 - HARBOR:%u\n",HARBOR);
    skynet_harbor_init(1); // 1<<24 = 2^24 = 16777216
    printf("harbor 2 - HARBOR:%u\n",HARBOR);
     return 0;
}

//harbor 1 - HARBOR:-1
//harbor 2 - HARBOR:4294967295
//harbor 2 - HARBOR:16777216


//--------------------------------------------------------------------------