
#include <stdio.h>

#include "logger.h"


int main(int argc, const char** argv)
{
	int i = 0;
	LOG_DEBUG("hello world - %d.",i);
	while (1);
	return 0;
}

