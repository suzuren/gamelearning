
#include <stdio.h>

#include <memory.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


void get_next(char *T, int next[])   //修正前的next数组
{
	int i = 1, j = 0;
	next[0] = -1;
	next[1] = 0;
	int m = (int)strlen(T);
	while (i < m - 1)
	{
		if (j == -1 || T[j] == T[i])
		{
			++i;
			++j;
			next[i] = j;
		}
		else
		{
			j = next[j];
		}
	}
}

void get_nextval(char *T, int nextval[]) //修正后的nextval数组
{
	int i = 1, j = 0;
	nextval[0] = -1;
	nextval[1] = 0;
	int m = (int)strlen(T);
	while (i < m - 1)
	{
		if (j == -1 || T[j] == T[i])
		{
			++i;
			++j;
			if (T[i] != T[j])
			{
				nextval[i] = j;
			}
			else
			{
				nextval[i] = nextval[j];
			}
		}
		else
		{
			j = nextval[j];
		}
	}
}

int Index_kmp(char *S, char *T, int pos, int next[])   //逐项比较
{
	if (S==NULL || T==NULL)
	{
		return - 1;
	}
	int j = 0, i = pos, lens = strlen(S), lent = strlen(T);
	get_next(T, next);
	while (i < lens && j < lent)
	{
		if (S[i] == T[j] || j == -1)
		{
			i++; j++;
		}
		else
		{
			j = next[j];
		}
	}
	if (j >= lent)
	{
		return i - lent;
	}
	else
	{
		return -1;
	}
}


/**
* build a table for the word to be searched
* eg:
* i		0 	1 	2 	3 	4 	5 	6
* W[i] 	A 	B 	C 	D 	A 	B 	D
* T[i] 	-1 	0 	0 	0 	0 	1 	2
*/
void kmp_table(const char *W, int * T, int len)
{
	int pos = 2; // the current position we are computing in T 
	int cnd = 0; // the next character of the current candidate substring
	T[0] = -1;
	T[1] = 0;

	while (pos < len)
	{
		// first case: the substring continues
		if (W[pos - 1] == W[cnd])
		{
			cnd++;
			T[pos] = cnd;
			pos++;
		}
		else if (cnd >0)
		{ // second case: it doesn't, but we can fall back
			cnd = T[cnd];
		}
		else
		{ // third case: we have run out of candidates.  Note cnd = 0
			T[pos] = 0;
			pos++;
		}
	}
}

int kmp_search(const char * S, const char * W)
{
	int LEN_S = (int)strlen(S);
	int LEN_W = (int)strlen(W);

	int m = 0;
	int i = 0;
	int T[LEN_W];

	kmp_table(W, T, LEN_W);

	while (m + i < LEN_S)
	{
		if (W[i] == S[m + i])
		{
			if (i == LEN_W - 1)
			{
				return m;
			}
			i++;
		}
		else
		{
			m = m + i - T[i];
			if (T[i] > -1)
			{
				i = T[i];
			}
			else
			{
				i = 0;
			}
		}
	}
	return -1;
}

int kmp_search_2(const char * S,int len_s,const char * W,int len_w)
{
	if (S == NULL || W == NULL || len_s <= 0 || len_w <= 0)
	{
		return -1;
	}
	int LEN_S = len_s;
	int LEN_W = len_w;

	int m = 0;
	int i = 0;
	int T[LEN_W];

	kmp_table(W, T, LEN_W);

	while (m + i < LEN_S)
	{
		if (W[i] == S[m + i])
		{
			if (i == LEN_W - 1)
			{
				return m;
			}
			i++;
		}
		else
		{
			m = m + i - T[i];
			if (T[i] > -1)
			{
				i = T[i];
			}
			else
			{
				i = 0;
			}
		}
	}
	return -1;
}

int get_http_response_body(char * pdata,int len_data, char *pbuff,int len_buff)
{
	char *plenght = (char *)"content-length: ";
	int len_w = (int)strlen(plenght);
	int pos_length = kmp_search_2(pdata, len_data, plenght, len_w);
	//int pos_length = kmp_search(pdata, plenght);
	int len_body = -1;
	printf("len_body:%d,pos_length:%d\n", len_body, pos_length);

	if (pos_length > 0)
	{
		char * pLenght = pdata + pos_length + len_w;
		int index = 0;
		char ltemp[8] = { 0 };
		bool bbody_flag = true;
		while (true)
		{
			if ((pLenght + index) != NULL)
			{
				char ch = (*(pLenght + index));
				if (isdigit(ch))
				{
					ltemp[index] = ch;
					index++;
				}
				else
				{
					break;
				}
				if (pos_length + len_w + index >= len_data)
				{
					bbody_flag = false;
					break;
				}
				if (index == 8)
				{
					bbody_flag = false;
					break;
				}
			}
			else
			{
				bbody_flag = false;
			}
		}
		int len_temp = atoi(ltemp);
		printf("bbody_flag:%d,len_temp:%d\n", bbody_flag, len_temp);
		if (bbody_flag && len_temp > 0)
		{
			char *pLineFeed = (char *)"\r\n\r\n";
			int len_line = (int)strlen(pLineFeed);
			int pos_line = kmp_search_2(pdata, len_data, pLineFeed, len_line);
			int len_src_body = len_data - (pos_line + len_line);
			int pos_body_start = pos_line + len_line;
			printf("pos_line:%d,len_src_body:%d,pos_body_start:%d,pdata+pos_body_start:%s-\n", pos_line, len_src_body, pos_body_start, pdata + pos_body_start);

			if (pos_line > 0 && len_src_body == len_temp && pos_body_start + len_src_body == len_data)
			{
				len_body = len_temp;
				if (len_buff >= len_temp)
				{
					memcpy(pbuff, pdata + pos_body_start,  len_temp);
				}
				else
				{
					len_body = 0;
				}
			}
		}
	}
	return len_body;
}

int test_KMP()
{
	printf("hello test_KMP!\n");

	/*
	char *S = "adbadabbaabadabbadada";
	char *T = "adabbadada";
	int m;
	int *next = (int *)malloc(((int)strlen(T) + 1)*sizeof(int));   //修正前的next数组
	int *nextval = (int *)malloc(((int)strlen(T) + 1)*sizeof(int));    //修正后的nextval数组

	get_next(T, next);
	printf("修正前next数组为：");
	for (m = 0; m<(int)strlen(T); m++)
	{
		printf("%d ", next[m] + 1);
	}

	get_nextval(T, nextval);
	printf("\n修正后的nextval数组为：");
	for (m = 0; m<(int)strlen(T); m++)
	{
		printf("%d ", nextval[m] + 1);
	}

	int t = Index_kmp(S, T, 0, nextval);
	if (t == -1) printf("\n无匹配项！\n");
	else
	{
		printf("\n在第%d项开始匹配成功\n", t + 1);
	}
	*/

	/*
	char *S = "HTTP/1.1 200 OK\r\nContent-type: application/json\r\ncontent-length:18\r\n\r\n{\"isSuccess\":true}";
	char *T = "content-length:";
	int m;
	int *next = (int *)malloc(((int)strlen(T) + 1)*sizeof(int));   //修正前的next数组
	int *nextval = (int *)malloc(((int)strlen(T) + 1)*sizeof(int));    //修正后的nextval数组

	get_next(T, next);
	printf("修正前next数组为：");
	for (m = 0; m<(int)strlen(T); m++)
	{
		printf("%d ", next[m] + 1);
	}

	get_nextval(T, nextval);
	printf("\n修正后的nextval数组为：");
	for (m = 0; m<(int)strlen(T); m++)
	{
		printf("%d ", nextval[m] + 1);
	}

	int t = Index_kmp(S, T, 0, nextval);
	if (t == -1) printf("\n无匹配项！\n");
	else
	{
		printf("\n在第%d项开始匹配成功\n", t);
		printf("S_kmp:%s-\n", S + (t));
	}

	char * temp_len = S + (t+strlen(T));
	printf("temp_len:%s-\n", temp_len);

	int index = 0;
	char len_buf[8] = { 0 };
	while (true)
	{
		char ch = (*(temp_len + index));
		if (isdigit(ch))
		{
			len_buf[index] = ch;
			index++;
		}
		else
		{
			break;
		}
	}
	int ilen = atoi(len_buf);
	index += strlen("\r\n\r\n");
	printf("ilen:%d,len_buf:%s-\n", ilen, len_buf);
	printf("index:%d,temp_len+index:%s-\n", index,temp_len + index);
	*/

	/*
hello test_KMP!
修正前next数组为：0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 
修正后的nextval数组为：0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 
在第49项开始匹配成功
S_kmp:content-length:18

{"isSuccess":true}-
temp_len:18

{"isSuccess":true}-
ilen:18,len_buf:18-
index:6,temp_len+index:{"isSuccess":true}-

	*/

	/*
	char *S = (char *)"HTTP/1.1 200 OK\r\nContent-type: application/json\r\ncontent-length:18\r\n\r\n{\"isSuccess\":true}";
	char *W = (char *)"content-length:";

	int pos = kmp_search(S, W);
	printf("to be searched:%s\n", W);

	if (pos > 0) {
		printf("found in pos:%d\n", pos);
		printf("text:\n%.*s", pos, S);
		printf("\033[31m%s\033[0m", W);
		printf("%s\n", &S[pos + strlen(W)]);
	}
	*/

	/*
	hello test!
	hello test_KMP!
	to be searched:content-length:
	found in pos:49
	text:
	HTTP/1.1 200 OK
	Content-type: application/json
	content-length:18

	{"isSuccess":true}
	*/

	static int max_body_szie = 1024;
	char buffer[max_body_szie];
	memset(buffer, 0, max_body_szie);
	char *pdata = (char *)"HTTP/1.1 200 OK\r\nContent-type: application/json\r\ncontent-length: 18\r\n\r\n{\"isSuccess\":true}";
	int len_data = (int)strlen(pdata);

	int len_body = get_http_response_body(pdata, len_data, buffer, max_body_szie);
	printf("len_data:%d,len_body:%d,buffer:%s-\n", len_data,len_body, buffer);


	return 0;
}
