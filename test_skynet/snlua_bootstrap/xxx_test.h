

void test_func()
{
	const char * cmdline = "snlua bootstrap";
	int sz = strlen(cmdline);
	char name[sz + 1];
	char args[sz + 1];
	sscanf(cmdline, "%s %s", name, args);

	printf("test_func - sz:%d,name:%s,args:%s\n", sz, name, args);
	//test_func - sz:15,name:snlua,args:bootstrap



}