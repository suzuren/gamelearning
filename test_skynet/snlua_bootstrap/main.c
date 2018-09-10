
#include <stdio.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "xxx_test.h"

//--------------------------------------------------------------------------
#define skynet_malloc malloc
#define skynet_free free


int main(int argc, char *argv[])
{
	

    printf("test skynet snlua bootstrap.\n");

	test_func();

     return 1;
}

//--------------------------------------------------------------------------