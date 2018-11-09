

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include "sproto.h"


static const char * pproto = " \
.Person{ \n\
	name 0 : string \n\
	id 1 : integer \n\
	email 2 : string \n\
 \n\
	.PhoneNumber{ \n\
	number 0 : string \n\
	type 1 : integer \n\
} \n\
\n\
phone 3 : *PhoneNumber \n\
} \n\
\n\
.AddressBook{ \n\
	person 0 : *Person(id) \n\
	others 1 : *Person \n\
} \n\
";


struct sproto *
lnewproto()
{
	struct sproto * sp = NULL;
	size_t sz = strlen(pproto);
	void * buffer = (void *)pproto;
	sp = sproto_create(buffer, sz);
	return sp;
}

int
ldeleteproto(struct sproto * sp)
{
	if (sp == NULL)
	{
		return 1;
	}
	sproto_release(sp);
	return 0;
}

int main()
{
	printf("hello world. test sproto ...\n");

	struct sproto * sp = lnewproto();
	//printf("struct sproto sp - sp:%p\n", sp);

	sproto_dump(sp);

	ldeleteproto(sp);
	return 0;
}

