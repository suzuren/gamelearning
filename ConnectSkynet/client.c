#include "select.h"
#include "pbc/pbc.h"
#include "readfile.h"

#include <arpa/inet.h>

#include <stdio.h>
#include <stdbool.h>


static struct pbc_wmessage * test_s2c_login_wmessage(struct pbc_env * env);
static void test_s2c_login_rmessage(struct pbc_env *env, struct pbc_slice *slice);
static struct pbc_wmessage * test_c2s_login_wmessage(struct pbc_env * env);
static void test_c2s_login_rmessage(struct pbc_env *env, struct pbc_slice *slice);
static bool CheckStorageMode();



int main(int argc, char const *argv[])
{
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



static struct pbc_wmessage *
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

static void
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

static void
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