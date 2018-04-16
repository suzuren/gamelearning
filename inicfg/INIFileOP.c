/*********************************************************************
* ��Ȩ���� (C)2005, ����ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� INIFileOP.C
* �ļ���ʶ�� 
* ����˵���� INI�ļ��Ĳ���
* ��ǰ�汾�� V1.0
* ��    �ߣ� 
* ������ڣ� 
*
* �޸ļ�¼1��
*    �޸����ڣ�2005��6��23��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�
*    �޸����ݣ�����
**********************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/*#undef  VOS_WINNT*/
#define  VOS_WINNT
#ifdef VOS_WINNT
#define INIFileTstmain main
#endif
#define SuccessRet 1;
#define FailedRet  0;
#define MAX_CFG_BUF                              512
#define CFG_OK                                   0 
#define CFG_SECTION_NOT_FOUND                    -1 
#define CFG_KEY_NOT_FOUND                        -2 
#define CFG_ERR                                  -10 
#define CFG_ERR_FILE                             -10 
#define CFG_ERR_OPEN_FILE                        -10 
#define CFG_ERR_CREATE_FILE                      -11 
#define CFG_ERR_READ_FILE                        -12 
#define CFG_ERR_WRITE_FILE                       -13 
#define CFG_ERR_FILE_FORMAT                      -14 
#define CFG_ERR_SYSTEM                           -20 
#define CFG_ERR_SYSTEM_CALL                      -20 
#define CFG_ERR_INTERNAL                         -21 
#define CFG_ERR_EXCEED_BUF_SIZE                  -22
#define COPYF_OK                                 0 
#define COPYF_ERR_OPEN_FILE                      -10 
#define COPYF_ERR_CREATE_FILE                    -11 
#define COPYF_ERR_READ_FILE                      -12 
#define COPYF_ERR_WRITE_FILE                     -13
#define TXTF_OK                                  0 
#define TXTF_ERR_OPEN_FILE                       -1 
#define TXTF_ERR_READ_FILE                       -2 
#define TXTF_ERR_WRITE_FILE                      -3 
#define TXTF_ERR_DELETE_FILE                     -4 
#define TXTF_ERR_NOT_FOUND                       -5
char CFG_ssl = '[', CFG_ssr = ']';  /* ���־��Section Symbol 
--�ɸ���������Ҫ���ж�����ģ��� { }��*/
char CFG_nis = ':';                 /*name �� index ֮��ķָ��� */
char CFG_nts = '#';                 /*ע�ͷ�*/
int  CFG_section_line_no, CFG_key_line_no, CFG_key_lines;
static char * strtrimr(char * buf);
static char * strtriml(char * buf);
static int  FileGetLine(FILE *fp, char *buffer, int maxlen); 
static int  SplitKeyValue(char *buf, char **key, char **val);
extern int  FileCopy(void *source_file, void *dest_file); 
extern int  SplitSectionToNameIndex(char *section, char **name, char **index);
extern int  JoinNameIndexToSection(char **section, char *name, char *index);
extern int  ConfigGetKey(void *CFG_file, void *section, void *key, void *buf); 
extern int  ConfigSetKey(void *CFG_file, void *section, void *key, void *buf); 
extern int  ConfigGetSections(void *CFG_file, char *sections[]); 
extern int  ConfigGetKeys(void *CFG_file, void *section, char *keys[]);
extern int INIFileTstmain(void);
/**********************************************************************
* �������ƣ� strtrimr
* ���������� ȥ���ַ����ұߵĿ��ַ�
* ���ʵı� ��
* �޸ĵı� ��
* ��������� char * buf �ַ���ָ��
* ��������� ��
* �� �� ֵ�� �ַ���ָ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2005/6/23        V1.0              ����
* 
***********************************************************************/
char * strtrimr(char * buf)
{
	int len, i;
	char * tmp = NULL;
	len = strlen(buf);
	tmp = (char*)malloc(len);

	memset(tmp, 0x00, len);
	for (i = 0; i < len; i++)
	{
		if (buf[i] != ' ')
			break;
	}
	if (i < len) {
		strncpy(tmp, (buf + i), (len - i));
	}
	strncpy(buf, tmp, len);
	free(tmp);
	return buf;
}
/**********************************************************************
* �������ƣ� strtriml
* ���������� ȥ���ַ�����ߵĿ��ַ�
* ���ʵı� ��
* �޸ĵı� ��
* ��������� char * buf �ַ���ָ��
* ��������� ��
* �� �� ֵ�� �ַ���ָ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2005/6/23        V1.0              ����
* 
***********************************************************************/
char * strtriml(char * buf)
{
	int len, i;
	char * tmp = NULL;
	len = strlen(buf);
	tmp = (char*)malloc(len);
	memset(tmp, 0x00, len);
	for (i = 0; i < len; i++)
	{
		if (buf[len - i - 1] != ' ')
			break;
	}
	if (i < len) {
		strncpy(tmp, buf, len - i);
	}
	strncpy(buf, tmp, len);
	free(tmp);
	return buf;
}
/**********************************************************************
* �������ƣ� FileGetLine
* ���������� ���ļ��ж�ȡһ��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� FILE *fp �ļ������int maxlen ��������󳤶�
* ��������� char *buffer һ���ַ���
* �� �� ֵ�� ʵ�ʶ��ĳ���
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2005/6/23        V1.0              ����
* 
***********************************************************************/
int  FileGetLine(FILE *fp, char *buffer, int maxlen)
{
	int  i, j;
	char ch1;

	for (i = 0, j = 0; i < maxlen; j++)
	{
		if (fread(&ch1, sizeof(char), 1, fp) != 1)
		{
			if (feof(fp) != 0)
			{
				if (j == 0) return -1;               /* �ļ����� */
				else break;
			}
			if (ferror(fp) != 0) return -2;        /* ���ļ����� */
			return -2;
		}
		else
		{
			if (ch1 == '\n' || ch1 == 0x00) break; /* ���� */
			if (ch1 == '\f' || ch1 == 0x1A)        /* '\f':��ҳ��Ҳ����Ч�ַ� */
			{
				buffer[i++] = ch1;
				break;
			}
			if (ch1 != '\r') buffer[i++] = ch1;    /* ���Իس��� */
		}
	}
	buffer[i] = '\0';
	return i;
}
/**********************************************************************
* �������ƣ� FileCopy
* ���������� �ļ�����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� void *source_file��Դ�ļ���void *dest_file��Ŀ���ļ�
* ��������� ��
* �� �� ֵ�� 0 -- OK,��0����ʧ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2005/6/23        V1.0              ����
* 
***********************************************************************/
int  FileCopy(void *source_file, void *dest_file)
{
	FILE *fp1, *fp2;
	char buf[1024 + 1];
	int  ret;

	if ((fp1 = fopen((char *)source_file, "r")) == NULL)
		return COPYF_ERR_OPEN_FILE;
	ret = COPYF_ERR_CREATE_FILE;

	if ((fp2 = fopen((char *)dest_file, "w")) == NULL) goto copy_end;

	while (1)
	{
		ret = COPYF_ERR_READ_FILE;
		memset(buf, 0x00, 1024 + 1);
		if (fgets((char *)buf, 1024, fp1) == NULL)
		{
			if (strlen(buf) == 0)
			{
				if (ferror(fp1) != 0) goto copy_end;
				break;                                   /* �ļ�β */
			}
		}
		ret = COPYF_ERR_WRITE_FILE;
		if (fputs((char *)buf, fp2) == EOF) goto copy_end;
	}
	ret = COPYF_OK;
copy_end:
	if (fp2 != NULL) fclose(fp2);
	if (fp1 != NULL) fclose(fp1);
	return ret;
}
/**********************************************************************
* �������ƣ� SplitSectionToNameIndex
* ���������� ����sectionΪname��index
*            [section]
*              /   \
*            name:index
*            jack  :   12 
*         |     |   | 
*            k1    k2  i 
* ���ʵı� ��
* �޸ĵı� ��
* ��������� char *section  
* ��������� char **name, char **index
* �� �� ֵ�� 1 --- ok 
*    0 --- blank line 
*   -1 --- no name, ":index" 
*   -2 --- only name, no ':' 
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2005/6/23        V1.0              ����
* 
***********************************************************************/
int  SplitSectionToNameIndex(char *section, char **name, char **index)
{
	int  i, k1, k2, n;

	if ((n = strlen((char *)section)) < 1) return 0;
	for (i = 0; i < n; i++)
		if (section[i] != ' ' && section[i] != '\t') break;
	if (i >= n) return 0;
	if (section[i] == CFG_nis) return -1;
	k1 = i;
	for (i++; i < n; i++)
		if (section[i] == CFG_nis) break;
	if (i >= n) return -2;
	k2 = i;
	for (i++; i < n; i++)
		if (section[i] != ' ' && section[i] != '\t') break;
	section[k2] = '\0';
	*name = section + k1;
	*index = section + i;
	return 1;
}
/**********************************************************************
* �������ƣ� JoinNameIndexToSection
* ���������� �ϳ�name��indexsectionΪsection
*            jack  :   12 
*            name:index
*              \   /
*            [section]
* ���ʵı� ��
* �޸ĵı� ��
* ��������� char *name, char *index
* ��������� char **section  
* �� �� ֵ�� 1 --- ok 
*    0 --- blank line 
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2005/6/23        V1.0              ����
* 
***********************************************************************/
int  JoinNameIndexToSection(char **section, char *name, char *index)
{
	int n1, n2;

	if ((n1 = strlen((char *)name)) < 1) return 0;
	if ((n2 = strlen((char *)index)) < 1) return 0;

	strcat(*section, name);
	strcat(*section + n1, ":");
	strcat(*section + n1 + 1, index);
	*(*section + n1 + 1 + n2) = '\0';
	return 1;
}
/**********************************************************************
* �������ƣ� SplitKeyValue
* ���������� ����key��value
*������������key=val
*   jack   =   liaoyuewang 
*   |      |   | 
*   k1     k2  i 
* ���ʵı� ��
* �޸ĵı� ��
* ��������� char *buf  
* ��������� char **key;char **val
* �� �� ֵ�� 1 --- ok 
*    0 --- blank line 
*   -1 --- no key, "= val" 
*   -2 --- only key, no '=' 
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2005/6/23        V1.0              ����
* 
***********************************************************************/
int  SplitKeyValue(char *buf, char **key, char **val)
{
	int  i, k1, k2, n;

	if ((n = strlen((char *)buf)) < 1) return 0;
	for (i = 0; i < n; i++)
		if (buf[i] != ' ' && buf[i] != '\t') break;
	if (i >= n) return 0;
	if (buf[i] == '=') return -1;
	k1 = i;
	for (i++; i < n; i++)
		if (buf[i] == '=') break;
	if (i >= n) return -2;
	k2 = i;
	for (i++; i < n; i++)
		if (buf[i] != ' ' && buf[i] != '\t') break;
	buf[k2] = '\0';
	*key = buf + k1;
	*val = buf + i;
	return 1;
}
/**********************************************************************
* �������ƣ� ConfigGetKey
* ���������� ���key��ֵ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� void *CFG_file���ļ���void *section����ֵ��void *key����ֵ
* ��������� void *buf��key��ֵ
* �� �� ֵ�� 0 --- ok ��0 --- error 
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2005/6/23        V1.0              ����
* 
***********************************************************************/
int  ConfigGetKey(void *CFG_file, void *section, void *key, void *buf)
{
	FILE *fp;
	char buf1[MAX_CFG_BUF + 1], buf2[MAX_CFG_BUF + 1];
	char *key_ptr, *val_ptr;
	int  line_no, n, ret;

	line_no = 0;
	CFG_section_line_no = 0;
	CFG_key_line_no = 0;
	CFG_key_lines = 0;

	if ((fp = fopen((char *)CFG_file, "rb")) == NULL) return CFG_ERR_OPEN_FILE;

	while (1)                                       /* ������section */
	{
		ret = CFG_ERR_READ_FILE;
		n = FileGetLine(fp, buf1, MAX_CFG_BUF);
		if (n < -1) goto r_cfg_end;
		ret = CFG_SECTION_NOT_FOUND;
		if (n < 0) goto r_cfg_end;                    /* �ļ�β��δ���� */
		line_no++;
		n = strlen(strtriml(strtrimr(buf1)));
		if (n == 0 || buf1[0] == CFG_nts) continue;       /* ���� �� ע���� */
		ret = CFG_ERR_FILE_FORMAT;
		if (n > 2 && ((buf1[0] == CFG_ssl && buf1[n - 1] != CFG_ssr)))
			goto r_cfg_end;
		if (buf1[0] == CFG_ssl)
		{
			buf1[n - 1] = 0x00;
			if (strcmp(buf1 + 1, (const char*)section) == 0)
				break;                                   /* �ҵ���section */
		}
	}
	CFG_section_line_no = line_no;
	while (1)                                       /* ����key */
	{
		ret = CFG_ERR_READ_FILE;
		n = FileGetLine(fp, buf1, MAX_CFG_BUF);
		if (n < -1) goto r_cfg_end;
		ret = CFG_KEY_NOT_FOUND;
		if (n < 0) goto r_cfg_end;                    /* �ļ�β��δ����key */
		line_no++;
		CFG_key_line_no = line_no;
		CFG_key_lines = 1;
		n = strlen(strtriml(strtrimr(buf1)));
		if (n == 0 || buf1[0] == CFG_nts) continue;       /* ���� �� ע���� */
		ret = CFG_KEY_NOT_FOUND;
		if (buf1[0] == CFG_ssl) goto r_cfg_end;
		if (buf1[n - 1] == '+')                         /* ��+�ű�ʾ��һ�м���  */
		{
			buf1[n - 1] = 0x00;
			while (1)
			{
				ret = CFG_ERR_READ_FILE;
				n = FileGetLine(fp, buf2, MAX_CFG_BUF);
				if (n < -1) goto r_cfg_end;
				if (n < 0) break;                         /* �ļ����� */
				line_no++;
				CFG_key_lines++;
				n = strlen(strtrimr(buf2));
				ret = CFG_ERR_EXCEED_BUF_SIZE;
				if (n > 0 && buf2[n - 1] == '+')            /* ��+�ű�ʾ��һ�м��� */
				{
					buf2[n - 1] = 0x00;
					if (strlen(buf1) + strlen(buf2) > MAX_CFG_BUF)
						goto r_cfg_end;
					strcat(buf1, buf2);
					continue;
				}
				if (strlen(buf1) + strlen(buf2) > MAX_CFG_BUF)
					goto r_cfg_end;
				strcat(buf1, buf2);
				break;
			}
		}
		ret = CFG_ERR_FILE_FORMAT;
		if (SplitKeyValue(buf1, &key_ptr, &val_ptr) != 1)
			goto r_cfg_end;
		strtriml(strtrimr(key_ptr));
		if (strcmp(key_ptr, (const char*)key) != 0)
			continue;                                  /* ��keyֵ��ƥ�� */
		strcpy((char *)buf, val_ptr);
		break;
	}
	ret = CFG_OK;
r_cfg_end:
	if (fp != NULL) fclose(fp);
	return ret;
}
/**********************************************************************
* �������ƣ� ConfigSetKey
* ���������� ����key��ֵ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� void *CFG_file���ļ���void *section����ֵ��
*     void *key����ֵ��void *buf��key��ֵ
* ��������� ��
* �� �� ֵ��   0 --- ok ��0 --- error 
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2005/6/23        V1.0              ����
* 
***********************************************************************/
int  ConfigSetKey(void *CFG_file, void *section, void *key, void *buf)
{
	FILE *fp1, *fp2;
	char buf1[MAX_CFG_BUF + 1];
	int  line_no, line_no1, n, ret, ret2;
	//char *tmpfname;
	char tmpfname[256] = { 0 };
	ret = ConfigGetKey(CFG_file, section, key, buf1);
	if (ret <= CFG_ERR && ret != CFG_ERR_OPEN_FILE) return ret;
	if (ret == CFG_ERR_OPEN_FILE || ret == CFG_SECTION_NOT_FOUND)
	{

		if ((fp1 = fopen((char *)CFG_file, "a")) == NULL)

			return CFG_ERR_CREATE_FILE;

		if (fprintf(fp1, "%c%s%c\n", CFG_ssl, (char*)section, CFG_ssr) == EOF)
		{
			fclose(fp1);
			return CFG_ERR_WRITE_FILE;
		}
		if (fprintf(fp1, "%s=%s\n", (char*)key, (char*)buf) == EOF)
		{
			fclose(fp1);
			return CFG_ERR_WRITE_FILE;
		}
		fclose(fp1);
		return CFG_OK;
	}
	//warning: the use of `tmpnam' is dangerous, better use `mkstemp'
	//if ((tmpfname = tmpnam(NULL)) == NULL)
	//	return CFG_ERR_CREATE_FILE;
	if ((mkstemp(tmpfname)) == -1)
		return CFG_ERR_CREATE_FILE;

	if ((fp2 = fopen(tmpfname, "w")) == NULL)

		return CFG_ERR_CREATE_FILE;
	ret2 = CFG_ERR_OPEN_FILE;

	if ((fp1 = fopen((char *)CFG_file, "rb")) == NULL) goto w_cfg_end;

	if (ret == CFG_KEY_NOT_FOUND)
		line_no1 = CFG_section_line_no;
	else /* ret = CFG_OK */
		line_no1 = CFG_key_line_no - 1;
	for (line_no = 0; line_no < line_no1; line_no++)
	{
		ret2 = CFG_ERR_READ_FILE;
		n = FileGetLine(fp1, buf1, MAX_CFG_BUF);
		if (n < 0) goto w_cfg_end;
		ret2 = CFG_ERR_WRITE_FILE;
		if (fprintf(fp2, "%s\n", buf1) == EOF) goto w_cfg_end;
	}
	if (ret != CFG_KEY_NOT_FOUND)
		for (; line_no < line_no1 + CFG_key_lines; line_no++)
		{
			ret2 = CFG_ERR_READ_FILE;
			n = FileGetLine(fp1, buf1, MAX_CFG_BUF);
			if (n < 0) goto w_cfg_end;
		}
	ret2 = CFG_ERR_WRITE_FILE;
	if (fprintf(fp2, "%s=%s\n", (char*)key, (char*)buf) == EOF) goto w_cfg_end;
	while (1)
	{
		ret2 = CFG_ERR_READ_FILE;
		n = FileGetLine(fp1, buf1, MAX_CFG_BUF);
		if (n < -1) goto w_cfg_end;
		if (n < 0) break;
		ret2 = CFG_ERR_WRITE_FILE;
		if (fprintf(fp2, "%s\n", buf1) == EOF) goto w_cfg_end;
	}
	ret2 = CFG_OK;
w_cfg_end:
	if (fp1 != NULL) fclose(fp1);
	if (fp2 != NULL) fclose(fp2);
	if (ret2 == CFG_OK)
	{
		ret = FileCopy(tmpfname, CFG_file);
		if (ret != 0) return CFG_ERR_CREATE_FILE;
	}
	remove(tmpfname);
	return ret2;
}
/**********************************************************************
* �������ƣ� ConfigGetSections
* ���������� �������section
* ���ʵı� ��
* �޸ĵı� ��
* ��������� void *CFG_file���ļ�
* ��������� char *sections[]�����section����
* �� �� ֵ�� ����section���������������ظ�����
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2005/6/23        V1.0              ����
* 
***********************************************************************/
int  ConfigGetSections(void *CFG_file, char *sections[])
{
	FILE *fp;
	char buf1[MAX_CFG_BUF + 1];
	int  n, n_sections = 0, ret;


	if ((fp = fopen((const char *)CFG_file, "rb")) == NULL) return CFG_ERR_OPEN_FILE;

	while (1)                                       /*������section */
	{
		ret = CFG_ERR_READ_FILE;
		n = FileGetLine(fp, buf1, MAX_CFG_BUF);
		if (n < -1) goto cfg_scts_end;
		if (n < 0) break;                             /* �ļ�β */
		n = strlen(strtriml(strtrimr(buf1)));
		if (n == 0 || buf1[0] == CFG_nts) continue;       /* ���� �� ע���� */
		ret = CFG_ERR_FILE_FORMAT;
		if (n > 2 && ((buf1[0] == CFG_ssl && buf1[n - 1] != CFG_ssr)))
			goto cfg_scts_end;
		if (buf1[0] == CFG_ssl)
		{
			buf1[n - 1] = 0x00;
			strcpy(sections[n_sections], buf1 + 1);
			n_sections++;
		}
	}
	ret = n_sections;
cfg_scts_end:
	if (fp != NULL) fclose(fp);
	return ret;
}
/**********************************************************************
* �������ƣ� ConfigGetKeys
* ���������� �������key�����֣�key=value��ʽ, value���üӺű�ʾ���У�
* ���ʵı� ��
* �޸ĵı� ��
* ��������� void *CFG_file���ļ� void *section ��ֵ
* ��������� char *keys[]�����key����
* �� �� ֵ�� ����key���������������ظ�����
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2005/6/23        V1.0              ����
* 
***********************************************************************/
int  ConfigGetKeys(void *CFG_file, void *section, char *keys[])
{
	FILE *fp;
	char buf1[MAX_CFG_BUF + 1], buf2[MAX_CFG_BUF + 1];
	char *key_ptr, *val_ptr;
	int  n, n_keys = 0, ret;


	if ((fp = fopen((const char*)CFG_file, "rb")) == NULL) return CFG_ERR_OPEN_FILE;

	while (1)                                       /* ������section */
	{
		ret = CFG_ERR_READ_FILE;
		n = FileGetLine(fp, buf1, MAX_CFG_BUF);
		if (n < -1) goto cfg_keys_end;
		ret = CFG_SECTION_NOT_FOUND;
		if (n < 0) goto cfg_keys_end;                 /* �ļ�β */
		n = strlen(strtriml(strtrimr(buf1)));
		if (n == 0 || buf1[0] == CFG_nts) continue;       /* ���� �� ע���� */
		ret = CFG_ERR_FILE_FORMAT;
		if (n > 2 && ((buf1[0] == CFG_ssl && buf1[n - 1] != CFG_ssr)))
			goto cfg_keys_end;
		if (buf1[0] == CFG_ssl)
		{
			buf1[n - 1] = 0x00;
			if (strcmp(buf1 + 1, (const char*)section) == 0)
				break;                                   /* �ҵ���section */
		}
	}
	while (1)
	{
		ret = CFG_ERR_READ_FILE;
		n = FileGetLine(fp, buf1, MAX_CFG_BUF);
		if (n < -1) goto cfg_keys_end;
		if (n < 0) break;                             /* �ļ�β */
		n = strlen(strtriml(strtrimr(buf1)));
		if (n == 0 || buf1[0] == CFG_nts) continue;       /* ���� �� ע���� */
		ret = CFG_KEY_NOT_FOUND;
		if (buf1[0] == CFG_ssl)
			break;                                     /* ��һ�� section */
		if (buf1[n - 1] == '+')                         /* ��+�ű�ʾ��һ�м��� */
		{
			buf1[n - 1] = 0x00;
			while (1)
			{
				ret = CFG_ERR_READ_FILE;
				n = FileGetLine(fp, buf2, MAX_CFG_BUF);
				if (n < -1) goto cfg_keys_end;
				if (n < 0) break;                         /* �ļ�β */
				n = strlen(strtrimr(buf2));
				ret = CFG_ERR_EXCEED_BUF_SIZE;
				if (n > 0 && buf2[n - 1] == '+')            /* ��+�ű�ʾ��һ�м��� */
				{
					buf2[n - 1] = 0x00;
					if (strlen(buf1) + strlen(buf2) > MAX_CFG_BUF)
						goto cfg_keys_end;
					strcat(buf1, buf2);
					continue;
				}
				if (strlen(buf1) + strlen(buf2) > MAX_CFG_BUF)
					goto cfg_keys_end;
				strcat(buf1, buf2);
				break;
			}
		}
		ret = CFG_ERR_FILE_FORMAT;
		if (SplitKeyValue(buf1, &key_ptr, &val_ptr) != 1)
			goto cfg_keys_end;
		strtriml(strtrimr(key_ptr));
		strcpy(keys[n_keys], key_ptr);
		n_keys++;
	}
	ret = n_keys;
cfg_keys_end:
	if (fp != NULL) fclose(fp);
	return ret;
}
/**********************************************************************
* �������ƣ� INIFileTstmain
* ���������� ���Ժ������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2005/6/23        V1.0              ����
* 
***********************************************************************/
int INIFileTstmain(void)
{
	char buf[20] = "";
	char buf1[20] = "";
	char buf2[20] = "";
	char buf3[20] = "";
	int  ret = 0;
	long abc;
	int i;
	if (ret == 0)
	{
		//
	}
	char * section;
	char * key;
	char * val;
	char * name;
	char * index;
	section = buf1;
	key = buf2;
	val = buf3;

	ret = ConfigSetKey((void*)"Config.dat", (void*)"Jack:Lio", (void*) "Jack", (void*)"-12321");

	for (i = 0; i < 20; i++)
	{
		memset(buf, 0x00, 20);
		memset(buf1, 0x00, 20);
		memset(buf2, 0x00, 20);
		memset(buf3, 0x00, 20);
		sprintf(buf, "%d", i);
		sprintf(buf2, "Lio%d", i);
		sprintf(buf3, "%d", i);
		JoinNameIndexToSection(&section, (char*)"Jack", buf);
		ConfigSetKey((void*)"Config.dat", section, key, val);
	}

	name = buf2;
	index = buf3;
	memset(buf1, 0x00, 20);
	memset(buf2, 0x00, 20);
	memset(buf3, 0x00, 20);
	strcpy(buf1, "Jack:Lio");
	SplitSectionToNameIndex(section, &name, &index);
	printf("\n name=%s,index=%s\n", name, index);

	ret = ConfigGetKey((void*)"Config.dat", (void*)"Jack:Lio", (void*)"Jack", buf);
	if (strcmp(buf, "") != 0) {
		abc = atol(buf);
	}

	printf("\n buf=%s\n", buf);
	printf("\n abc=%ld\n", abc);

	ConfigSetKey((void*)"Config.dat", (void*)"Jack:Lio", (void*)"Email", (void*)"liaoyuewang@163.com");
	FileCopy((void*)"Config.dat", (void*)"Configbak.dat");
	printf("\nFile %s line%d\n", __FILE__, __LINE__);
	printf("\n******** This test is created by Jack Lio. Email:liaoyuewang@163.com********\n\n");
	return 0;
}