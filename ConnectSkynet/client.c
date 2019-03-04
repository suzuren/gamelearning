#include "select.h"
#include "pbc/pbc.h"
#include "readfile.h"

#include <arpa/inet.h>

#include <stdio.h>
#include <stdbool.h>


#include <sys/resource.h>


struct pbc_wmessage * test_s2c_login_wmessage(struct pbc_env * env);
void test_s2c_login_rmessage(struct pbc_env *env, struct pbc_slice *slice);
static struct pbc_wmessage * test_c2s_login_wmessage(struct pbc_env * env);
void test_c2s_login_rmessage(struct pbc_env *env, struct pbc_slice *slice);
bool CheckStorageMode();
static int test_pbc_write_read_data();


int main(int argc, char const *argv[])
{
	int ret_pbc_write_read_data = test_pbc_write_read_data();
	if (ret_pbc_write_read_data ==1)
	{
		return 0;
	}
	

	int client_fd = socket_connect(IPADDRESS, PORT);
	printf("socket_connect client_fd:%d\n", client_fd);

	
	struct pbc_slice slice;
	read_file("loginServer.login.c2s.pb", &slice);
	if (slice.buffer == NULL)
	{
		printf("read_file error\n");
		return 1;
	}
	struct pbc_env * env = pbc_new();
	int ret = pbc_register(env, &slice);
	if (ret)
	{
		printf("Error : %s", pbc_error(env));
		return 1;
	}
	free(slice.buffer);
	//printf("pbc_wmessage_buffer start\n");

	struct pbc_wmessage *msg = test_c2s_login_wmessage(env);
	pbc_wmessage_buffer(msg, &slice);

	printf("test_wmessage success\n");
	test_c2s_login_rmessage(env, &slice);
	printf("test_rmessage success\n");
	pbc_wmessage_delete(msg);
	pbc_delete(env);

	//int iCount = 0;

	//struct heard{
	//	unsigned short size;
	//};
	//struct heard * taghead = (struct heard *)write_buf;
	
	//int heardSize = sizeof(struct heard);

	//printf("1 pack_size:%d,heardSize:%d - %d\n", pack_size, heardSize, taghead->size);

	//if (CheckStorageMode())
	//{
	//	//taghead->size = pack_size;
	//	memcpy(write_buf , &pack_size, sizeof(pack_size));
	//}
	//else
	//{
	//	pack_size = htons(pack_size);
	//	memcpy(write_buf, &pack_size, sizeof(pack_size));
	//}

	//printf("2 pack_size:%d,heardSize:%d - %d\n", pack_size, heardSize, taghead->size);


	char write_buf[65535] = { 0 };

	unsigned int protoNo = 0x000100;
	unsigned short pack_size = 4 + slice.len;
	write_buf[0] = (pack_size >> 8) & 0xff;
	write_buf[1] = pack_size & 0xff;
	write_buf[2] = 0;
	write_buf[3] = (protoNo >> 16) & 0xff;
	write_buf[4] = (protoNo >> 8) & 0xff;
	write_buf[5] = protoNo & 0xff;

	memcpy(write_buf + 6, slice.buffer, slice.len);
	int total_len_send = 0;

	int k = 0;
	for (int i = 3; i<pack_size + 2; i++)
	{
		k += ((unsigned char)write_buf[i]);
		k &= 0xff;
	}

	printf("pack_size:%d,k:%d\n", pack_size, k);

	write_buf[2] = k;

	pack_size = 6 + slice.len;

	for (; pack_size>0;)
	{
		int len_buff = strlen(write_buf);
		int len_send = write(client_fd, write_buf + total_len_send, pack_size);

		if (len_send > 0)
		{
			pack_size -= len_send;
			total_len_send += len_send;

			printf("%s len_buff:%d,len_send:%d,pack_size:%d,total_len_send:%d,buf:%s\n", getStrTime(), len_buff, len_send, pack_size, total_len_send, write_buf);

		}
	}


	while (true)
	{
		//memset(write_buf, 0, sizeof(write_buf));
		//sprintf(write_buf, "%s %d\n", "hello world", iCount++);

		//int len_buff = strlen(write_buf);
		//int len_send = write(client_fd, write_buf, len_buff);
		//if (len_buff != len_send)
		//{
		//	printf("send buf error\n");
		//}

		//printf("%s len_buff:%d,len_send:%d,buf:%s\n", getStrTime(), len_buff, len_send, write_buf);

		char read_buf[65535] = { 0 };
		int nread = read(client_fd, read_buf, 65535);
		if (nread == 0)
		{
			close(client_fd);
			break;
		}
		else if (nread < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		{
			continue;
		}
		else
		{
			printf("recv - len:%ld,buf:%s.\n", strlen(read_buf), read_buf);
		}
		sleep(1);
	}


}




bool CheckStorageMode()
{
	int iFlag = 0x12345678;
	char *cByte = (char *)&iFlag;
	return(*cByte == 0x12);
}



struct pbc_wmessage *
test_s2c_login_wmessage(struct pbc_env * env)
{
	struct pbc_wmessage* w_msg = pbc_wmessage_new(env, "loginServer.login.s2c.Login");

	//printf("test_wmessage 1\n");

	pbc_wmessage_string(w_msg, "code", "RC_IP_BANNED", -1);
	pbc_wmessage_string(w_msg, "msg", "test string!", -1);

	//printf("test_wmessage 2\n");

	struct pbc_wmessage * data = pbc_wmessage_message(w_msg, "data");
	pbc_wmessage_integer(data, "userID", 12345, 0);
	pbc_wmessage_integer(data, "gameID", 12346, 0);
	pbc_wmessage_integer(data, "faceID", 32346, 0);
	pbc_wmessage_integer(data, "gender", 1, 0);
	pbc_wmessage_string(data, "nickName", "alice", -1);

	pbc_wmessage_integer(data, "isChangeNickName", 1, 0);

	//printf("test_wmessage 3\n");

	return w_msg;
}

void
test_s2c_login_rmessage(struct pbc_env *env, struct pbc_slice *slice)
{
	struct pbc_rmessage * r_msg = pbc_rmessage_new(env, "loginServer.login.s2c.Login", slice);
	if (r_msg == NULL)
	{
		printf("Error : %s", pbc_error(env));
		return;
	}
	int code_n = pbc_rmessage_size(r_msg, "code");
	printf("test_rmessage - code_n:%d\n", code_n);

	const char * pcode = pbc_rmessage_string(r_msg, "code", 0, NULL);
	printf("test_rmessage - pcode:%s\n", pcode);

	const char * pmsg = pbc_rmessage_string(r_msg, "msg", 0, NULL);
	printf("test_rmessage - pmsg:%s\n", pmsg);

	int data_n = pbc_rmessage_size(r_msg, "data");
	printf("test_rmessage - data_n:%d\n", data_n);

	struct pbc_rmessage * pdata = pbc_rmessage_message(r_msg, "data", 0);

	unsigned int userID = pbc_rmessage_integer(pdata, "userID", 0, NULL);
	unsigned int gameID = pbc_rmessage_integer(pdata, "gameID", 0, NULL);
	int faceID = pbc_rmessage_integer(pdata, "faceID", 0, NULL);
	int gender = pbc_rmessage_integer(pdata, "gender", 0, NULL);
	const char * pnickName = pbc_rmessage_string(pdata, "nickName", 0, NULL);
	int isRegister = pbc_rmessage_integer(pdata, "isRegister", 0, NULL);
	unsigned int memberOrder = pbc_rmessage_integer(pdata, "memberOrder", 0, NULL);
	const char * psignature = pbc_rmessage_string(pdata, "signature", 0, NULL);
	const char * pplatformFace = pbc_rmessage_string(pdata, "platformFace", 0, NULL);
	int isChangeNickName = pbc_rmessage_integer(pdata, "isChangeNickName", 0, NULL);

	printf("test_rmessage - userID:%d,gameID:%d,faceID:%d,gender:%d, pnickName:%s, isRegister:%d, memberOrder:%d, psignature:%s, pplatformFace:%s, isChangeNickName:%d\n", userID, gameID, faceID, gender, pnickName, isRegister, memberOrder, psignature, pplatformFace, isChangeNickName);

}


static struct pbc_wmessage *
test_c2s_login_wmessage(struct pbc_env * env)
{
	struct pbc_wmessage* w_msg = pbc_wmessage_new(env, "loginServer.login.c2s.Login");

	pbc_wmessage_string(w_msg, "session", "session test", -1);
	pbc_wmessage_string(w_msg, "nickName", "alice", -1);
	pbc_wmessage_string(w_msg, "machineID", "HYUInyuijh6", -1);
	pbc_wmessage_integer(w_msg, "kindID", 200, 0);
	pbc_wmessage_integer(w_msg, "scoreTag", 1, 0);
	pbc_wmessage_integer(w_msg, "appID", 201, 0);
	pbc_wmessage_string(w_msg, "appChannel", "appChannel test", -1);
	pbc_wmessage_string(w_msg, "appVersion", "appVersion test", -1);

	return w_msg;
}

void
test_c2s_login_rmessage(struct pbc_env *env, struct pbc_slice *slice)
{
	struct pbc_rmessage * r_msg = pbc_rmessage_new(env, "loginServer.login.c2s.Login", slice);
	if (r_msg == NULL)
	{
		printf("Error : %s", pbc_error(env));
		return;
	}

	const char * psession = pbc_rmessage_string(r_msg, "session", 0, NULL);
	const char * pnickName = pbc_rmessage_string(r_msg, "nickName", 0, NULL);
	const char * pmachineID = pbc_rmessage_string(r_msg, "machineID", 0, NULL);
	int kindID = pbc_rmessage_integer(r_msg, "kindID", 0, NULL);
	int scoreTag = pbc_rmessage_integer(r_msg, "scoreTag", 0, NULL);
	int appID = pbc_rmessage_integer(r_msg, "appID", 0, NULL);
	const char * pappChannel = pbc_rmessage_string(r_msg, "appChannel", 0, NULL);
	const char * pappVersion = pbc_rmessage_string(r_msg, "appVersion", 0, NULL);

	printf("test_rmessage - psession:%s, pnickName:%s, pmachineID:%s, kindID:%d, scoreTag:%d, appID:%d, pappChannel:%s, pappVersion:%s\n",
		psession, pnickName, pmachineID, kindID, scoreTag, appID, pappChannel, pappVersion);

}



int GenCoreDumpFile(size_t size)
{
	struct rlimit flimit;
	flimit.rlim_cur = size;
	flimit.rlim_max = size;
	if (setrlimit(RLIMIT_CORE, &flimit) != 0)
	{
		return errno;
	}
	return 0;
}

static void dump(uint8_t *buffer, int sz)
{
	int i, j;
	for (i = 0; i<sz; i++)
	{
		printf("%02X ", buffer[i]);
		if (i % 16 == 15)
		{
			for (j = 0; j <16; j++)
			{
				char c = buffer[i / 16 * 16 + j];
				if (c >= 32 && c<127)
				{
					printf("%c", c);
				}
				else
				{
					printf(".");
				}
			}
			printf("\n");
		}
	}

	printf("\n");
}

static int test_pbc_write_read_data()
{
	GenCoreDumpFile((uint32_t)(1024UL * 1024 * 1024 * 2));

	const char * file_path = "loginServer.login.s2c.pb";
	const char * type_name = "loginServer.login.s2c.Login";

	struct pbc_slice slice_write;
	read_file(file_path, &slice_write);
	if (slice_write.buffer == NULL)
	{
		printf("file_path:%s, read_file error\n", file_path);
		return 0;
	}
	struct pbc_env * env_write = pbc_new();
	if (env_write == NULL)
	{
		printf("file_path:%s, pbc_new error\n", file_path);
		return 0;
	}
	int ret_write = pbc_register(env_write, &slice_write);
	if (ret_write)
	{
		printf("file_path:%s, pbc_register Error : %s\n", file_path, pbc_error(env_write));
		return 0;
	}
	free(slice_write.buffer);

	struct pbc_wmessage * w_msg = pbc_wmessage_new(env_write, type_name);
	if (w_msg == NULL)
	{
		printf("file_path:%s, pbc_wmessage_new error\n", file_path);
		return 0;
	}

	pbc_wmessage_string(w_msg, "code", "RC_OK", strlen("RC_OK"));
	pbc_wmessage_string(w_msg, "msg", "success", strlen("success"));

	struct pbc_wmessage * pwrite_data = pbc_wmessage_message(w_msg, "data");
	pbc_wmessage_integer(pwrite_data, "userID", 10003, 0);
	pbc_wmessage_integer(pwrite_data, "gameID", 10008, 0);
	pbc_wmessage_integer(pwrite_data, "faceID", 10009, 0);
	pbc_wmessage_integer(pwrite_data, "gender", 10011, 0);
	pbc_wmessage_string(pwrite_data, "nickName", "alice", strlen("alice"));
	pbc_wmessage_integer(pwrite_data, "isRegister", 1, 0);
	pbc_wmessage_integer(pwrite_data, "memberOrder", 8, 0);
	pbc_wmessage_string(pwrite_data, "signature", "hello_world", strlen("hello_world"));
	pbc_wmessage_string(pwrite_data, "platformFace", "http_forest_url", strlen("http_forest_url"));
	pbc_wmessage_integer(pwrite_data, "isChangeNickName", 0, 0);
	pbc_wmessage_buffer(w_msg, &slice_write);

	//-------------------------------------------------------

	char read_buffer[65535] = { 0 };
	int read_lenght = 0;
	
	read_lenght = slice_write.len;
	memcpy(read_buffer, slice_write.buffer, read_lenght);

	pbc_wmessage_delete(w_msg);
	pbc_delete(env_write);

	printf("            read_lenght : %d\n", read_lenght);


	//-------------------------------------------------------

	
	struct pbc_slice slice_read;
	read_file(file_path, &slice_read);
	if (slice_read.buffer == NULL)
	{
		printf("read_file - file_path:%s,error\n", file_path);
		return 0;
	}
	struct pbc_env * env_read = pbc_new();
	if (env_read == NULL)
	{
		printf("pbc_new - file_path:%s,error\n", file_path);
		return 0;
	}
	int ret_read = pbc_register(env_read, &slice_read);
	if (ret_read)
	{
		printf("pbc_register - file_path:%s,error:%s\n", file_path, pbc_error(env_read));
		return 0;
	}
	
	free(slice_read.buffer);
	slice_read.buffer = read_buffer;
	slice_read.len = read_lenght;
	struct pbc_rmessage * r_msg = pbc_rmessage_new(env_read, type_name, &slice_read);
	if (r_msg == NULL)
	{
		printf("pbc_rmessage_new - file_path:%s,error:%s\n", file_path, pbc_error(env_read));
		return 0;
	}

	const char *  pread_code = pbc_rmessage_string(r_msg, "code", 0, NULL);
	const char *  pread_msg = pbc_rmessage_string(r_msg, "msg", 0, NULL);

	printf("             pread_code : %s\n", pread_code);
	printf("              pread_msg : %s\n", pread_msg);

	//int iread_size_data = pbc_rmessage_size(r_msg, "data");
	//for (int index = 0; index < iread_size_data; index++)
	//{
	//	const char *  pread_data = pbc_rmessage_message(r_msg, "data", index);
	//	int iread_userID = pbc_rmessage_integer(pread_data, "userID", index, NULL);
	//}

	struct pbc_rmessage *  pread_data = pbc_rmessage_message(r_msg, "data", 0);
	int iread_userID = pbc_rmessage_integer(pread_data, "userID", 0, NULL);
	int iread_gameID = pbc_rmessage_integer(pread_data, "gameID", 0, NULL);
	int iread_faceID = pbc_rmessage_integer(pread_data, "faceID", 0, NULL);
	int iread_gender = pbc_rmessage_integer(pread_data, "gender", 0, NULL);
	const char *  pread_nickName = pbc_rmessage_string(pread_data, "nickName", 0, NULL);
	int iread_isRegister = pbc_rmessage_integer(pread_data, "isRegister", 0, NULL);
	int iread_memberOrder = pbc_rmessage_integer(pread_data, "memberOrder", 0, NULL);
	const char *  pread_signature = pbc_rmessage_string(pread_data, "signature", 0, NULL);
	const char *  pread_platformFace = pbc_rmessage_string(pread_data, "platformFace", 0, NULL);
	int iread_isChangeNickName = pbc_rmessage_integer(pread_data, "isChangeNickName", 0, NULL);

	printf("           iread_userID : %d\n", iread_userID);
	printf("           iread_gameID : %d\n", iread_gameID);
	printf("           iread_faceID : %d\n", iread_faceID);
	printf("           iread_gender : %d\n", iread_gender);
	printf("         pread_nickName : %s\n", pread_nickName);
	printf("       iread_isRegister : %d\n", iread_isRegister);
	printf("      iread_memberOrder : %d\n", iread_memberOrder);
	printf("        pread_signature : %s\n", pread_signature);
	printf("     pread_platformFace : %s\n", pread_platformFace);
	printf(" iread_isChangeNickName : %d\n", iread_isChangeNickName);

	dump(slice_read.buffer, slice_read.len);


	pbc_rmessage_delete(r_msg);
	pbc_delete(env_read);
	
	return 1;
}



