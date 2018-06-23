#ifndef __BASE_UNIT_H_
#define __BASE_UNIT_H_

char* itoa_parser(int num, int radix)
{
	static char str[32];
	memset(str, 0, sizeof(str));
	static char index[17] = "0123456789ABCDEF";
	unsigned unum;//�м����
	int i = 0, j, k;
	//ȷ��unum��ֵ
	if (radix == 10 && num < 0)//ʮ���Ƹ���
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
	//����
	if (str[0] == '-')
	{
		k = 1;//ʮ���Ƹ���
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

const char* getStrTime()
{
	time_t now = time(0);
	struct tm * pTime = localtime(&now);

	static char szDate[32] = { 0 };

	sprintf(szDate, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec);

	return szDate;
}




#include <iconv.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <locale.h>

int code_convert(char *from_charset, char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	iconv_t cd;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset, from_charset);
	if (cd == 0)
		return -1;
	memset(outbuf, 0, outlen);
	if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
		return -1;
	iconv_close(cd);
	*pout = '\0';

	return 0;
}

int u2g(char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
	return code_convert("UTF-8", "GB2312", inbuf, inlen, outbuf, outlen);
}

int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("GBK", "UTF-8", inbuf, inlen, outbuf, outlen);
}



int gbk2utf8(char *utfstr, const char *srcstr, int maxutfstrlen)
{
	if (NULL == srcstr)
	{
		printf("bad parameter\n");
		return -1;
	}
	//�����Ƚ�gbk����ת��Ϊunicode����  
	if (NULL == setlocale(LC_ALL, "zh_cn.gbk"))//����ת��Ϊunicodeǰ����,��ǰΪgbk����  
	{
		printf("bad parameter\n");
		return -1;
	}
	int unicodelen = mbstowcs(NULL, srcstr, 0);//����ת����ĳ���  
	if (unicodelen <= 0)
	{
		printf("can not transfer!!!\n");
		return -1;
	}
	wchar_t *unicodestr = (wchar_t *)calloc(sizeof(wchar_t), unicodelen + 1);
	mbstowcs(unicodestr, srcstr, strlen(srcstr));//��gbkת��Ϊunicode  

												 //��unicode����ת��Ϊutf8����  
	if (NULL == setlocale(LC_ALL, "zh_cn.utf8"))//����unicodeת�������,��ǰΪutf8  
	{
		printf("bad parameter\n");
		return -1;
	}
	int utflen = wcstombs(NULL, unicodestr, 0);//����ת����ĳ���  
	if (utflen <= 0)
	{
		printf("can not transfer!!!\n");
		return -1;
	}
	else if (utflen >= maxutfstrlen)//�жϿռ��Ƿ��㹻  
	{
		printf("dst str memory not enough\n");
		return -1;
	}
	wcstombs(utfstr, unicodestr, utflen);
	utfstr[utflen] = 0;//��ӽ�����  
	free(unicodestr);
	return utflen;
}


#endif


