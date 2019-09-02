#ifndef __SUPPORT_ALGORITHM__
#define __SUPPORT_ALGORITHM__

#include <string.h>


void kmp_get_next(char *T, int next[])
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

void kmp_get_nextval(char *T, int nextval[])
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

int kmp_index(char *S, char *T, int pos, int next[])
{
	if (S == NULL || T == NULL)
	{
		return -1;
	}
	int j = 0, i = pos, lens = strlen(S), lent = strlen(T);
	kmp_get_next(T, next);
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

static inline int hextoint(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return 0;
}

int atoi_parser(int * ptrnum, char * ptrstr, int len)
{
	*ptrnum = 0;
	if (ptrnum == NULL || ptrstr == NULL || len == 0)
	{
		return 0;
	}
	for (int i = 0; i < len; i++)
	{
		(*ptrnum) += (hextoint(ptrstr[i]));
	}
	return 1;
}


char* itoa_parser(int num, int radix)
{
	static char str[32];
	memset(str, 0, sizeof(str));
	static char index[17] = "0123456789ABCDEF";
	unsigned unum;
	int i = 0, j, k;
	if (radix == 10 && num < 0)
	{
		unum = (unsigned)-num;
		str[i++] = '-';
	}
	else
	{
		unum = (unsigned)num;
	}
	do
	{
		str[i++] = index[unum % (unsigned)radix];
		unum /= radix;
	} while (unum);
	str[i] = '\0';
	if (str[0] == '-')
	{
		k = 1;
	}
	else
	{
		k = 0;
	}
	char temp;
	for (j = k; j <= (i - 1) / 2; j++)
	{
		temp = str[j];
		str[j] = str[i - 1 + k - j];
		str[i - 1 + k - j] = temp;
	}
	return str;
}


#endif
