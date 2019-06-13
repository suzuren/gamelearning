


extern "C" int  abs_f(int const number)
{
	return number >= 0 ? number : -number;
}

extern "C" long long  _abs64_f(long long const number)
{
	return number >= 0 ? number : -number;
}


/***
*strcmp - compare two strings, returning less than, equal to, or greater than
*
*Purpose:
*       STRCMP compares two strings and returns an integer
*       to indicate whether the first is less than the second, the two are
*       equal, or whether the first is greater than the second.
*
*       Comparison is done byte by byte on an UNSIGNED basis, which is to
*       say that Null (0) is less than any other character (1-255).
*
*Entry:
*       const char * src - string for left-hand side of comparison
*       const char * dst - string for right-hand side of comparison
*
*Exit:
*       returns -1 if src <  dst
*       returns  0 if src == dst
*       returns +1 if src >  dst
*
*Exceptions:
*
*******************************************************************************/

int strcmp_f(
	const char * src,
	const char * dst
	)
{
	int ret = 0;

	while ((ret = *(unsigned char *)src - *(unsigned char *)dst) == 0 && *dst)
		++src, ++dst;

	if (ret < 0)
		ret = -1;
	else if (ret > 0)
		ret = 1;

	return(ret);
}



/***
*strlen - return the length of a null-terminated string
*
*Purpose:
*       Finds the length in bytes of the given string, not including
*       the final null character.
*
*Entry:
*       const char * str - string whose length is to be computed
*
*Exit:
*       length of the string "str", exclusive of the final null byte
*
*Exceptions:
*
*******************************************************************************/

unsigned int strlen_f(
	const char * str
	)
{
	const char *eos = str;

	while (*eos++);

	return(eos - str - 1);
}

/***
*char *strncat(front, back, count) - append count chars of back onto front
*
*Purpose:
*       Appends at most count characters of the string back onto the
*       end of front, and ALWAYS terminates with a null character.
*       If count is greater than the length of back, the length of back
*       is used instead.  (Unlike strncpy, this routine does not pad out
*       to count characters).
*
*Entry:
*       char *front - string to append onto
*       char *back - string to append
*       unsigned count - count of max characters to append
*
*Exit:
*       returns a pointer to string appended onto (front).
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/

char * strncat_f(
	char * front,
	const char * back,
	unsigned int count
	)
{
	char *start = front;

	while (*front++)
		;
	front--;

	while (count--)
		if ((*front++ = *back++) != 0)
			return(start);

	*front = '\0';
	return(start);
}



/***
*int strncmp(first, last, count) - compare first count chars of strings
*
*Purpose:
*       Compares two strings for lexical order.  The comparison stops
*       after: (1) a difference between the strings is found, (2) the end
*       of the strings is reached, or (3) count characters have been
*       compared.
*
*Entry:
*       char *first, *last - strings to compare
*       unsigned count - maximum number of characters to compare
*
*Exit:
*       returns <0 if first < last
*       returns  0 if first == last
*       returns >0 if first > last
*
*Exceptions:
*
*******************************************************************************/

int strncmp_f
(
	const char *first,
	const char *last,
	unsigned int      count
	)
{
	unsigned int x = 0;

	if (!count)
	{
		return 0;
	}

	/*
	* This explicit guard needed to deal correctly with boundary
	* cases: strings shorter than 4 bytes and strings longer than
	* UINT_MAX-4 bytes .
	*/
	if (count >= 4)
	{
		/* unroll by four */
		for (; x < count - 4; x += 4)
		{
			first += 4;
			last += 4;

			if (*(first - 4) == 0 || *(first - 4) != *(last - 4))
			{
				return(*(unsigned char *)(first - 4) - *(unsigned char *)(last - 4));
			}

			if (*(first - 3) == 0 || *(first - 3) != *(last - 3))
			{
				return(*(unsigned char *)(first - 3) - *(unsigned char *)(last - 3));
			}

			if (*(first - 2) == 0 || *(first - 2) != *(last - 2))
			{
				return(*(unsigned char *)(first - 2) - *(unsigned char *)(last - 2));
			}

			if (*(first - 1) == 0 || *(first - 1) != *(last - 1))
			{
				return(*(unsigned char *)(first - 1) - *(unsigned char *)(last - 1));
			}
		}
	}

	/* residual loop */
	for (; x < count; x++)
	{
		if (*first == 0 || *first != *last)
		{
			return(*(unsigned char *)first - *(unsigned char *)last);
		}
		first += 1;
		last += 1;
	}

	return 0;
}

/***
*char *strncpy(dest, source, count) - copy at most n characters
*
*Purpose:
*       Copies count characters from the source string to the
*       destination.  If count is less than the length of source,
*       NO NULL CHARACTER is put onto the end of the copied string.
*       If count is greater than the length of sources, dest is padded
*       with null characters to length count.
*
*
*Entry:
*       char *dest - pointer to destination
*       char *source - source string for copy
*       unsigned count - max number of characters to copy
*
*Exit:
*       returns dest
*
*Exceptions:
*
*******************************************************************************/

char * strncpy_f(
	char * dest,
	const char * source,
	unsigned int count
	)
{
	char *start = dest;

	while (count && (*dest++ = *source++) != '\0')    /* copy string */
		count--;

	if (count)                              /* pad out with zeroes */
		while (--count)
			*dest++ = '\0';

	return(start);
}


/***
*char *_strrev(string) - reverse a string in place
*
*Purpose:
*       Reverses the order of characters in the string.  The terminating
*       null character remains in place.
*
*Entry:
*       char *string - string to reverse
*
*Exit:
*       returns string - now with reversed characters
*
*Exceptions:
*
*******************************************************************************/

char * _strrev_f(
	char * string
	)
{
	char *start = string;
	char *left = string;
	char ch;

	while (*string++)                 /* find end of string */
		;
	string -= 2;

	while (left < string)
	{
		ch = *left;
		*left++ = *string;
		*string-- = ch;
	}

	return(start);
}


/***
*char *_strset(string, val) - sets all of string to val
*
*Purpose:
*       Sets all of characters in string (except the terminating '/0'
*       character) equal to val.
*
*
*Entry:
*       char *string - string to modify
*       char val - value to fill string with
*
*Exit:
*       returns string -- now filled with val's
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/

char * _strset_f(
	char * string,
	int val
	)
{
	char *start = string;

	while (*string)
		*string++ = (char)val;

	return(start);
}

void test_c_func()
{

}
