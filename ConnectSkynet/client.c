#include "select.h"
#include "pbc/pbc.h"
#include "readfile.h"


static struct pbc_wmessage *
test_wmessage(struct pbc_env * env)
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
test_rmessage(struct pbc_env *env, struct pbc_slice *slice)
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

	printf("test_rmessage - userID:%d,gameID:%d,faceID:%d,gender:%d, pnickName:%s, isRegister:%d, memberOrder:%d, psignature:%s, pplatformFace:%s, isChangeNickName:%d\n",userID, gameID, faceID, gender, pnickName, isRegister, memberOrder, psignature, pplatformFace, isChangeNickName);

}


int main(int argc, char const *argv[])
{
	int client_fd = socket_connect(IPADDRESS, PORT);
	printf("socket_connect client_fd:%d\n", client_fd);

	
	struct pbc_slice slice;
	read_file("loginServer.login.s2c.pb", &slice);
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

	struct pbc_wmessage *msg = test_wmessage(env);
	pbc_wmessage_buffer(msg, &slice);

	printf("test_wmessage success\n");

	test_rmessage(env, &slice);

	printf("test_rmessage success\n");

	pbc_wmessage_delete(msg);
	pbc_delete(env);

	//int iCount = 0;
	char write_buf[65535] = { 0 };

	unsigned int protoNo = 0x000100;

	write_buf[1] = (protoNo >> 16) & 0xff;
	write_buf[2] = (protoNo >> 8) & 0xff;
	write_buf[3] = protoNo & 0xff;

	memcpy(write_buf + 4, slice.buffer, slice.len);
	int pack_size = 4 + slice.len;

	printf("pack_size:%d\n", pack_size);

	while (client_fd != -1)
	{
		//memset(write_buf, 0, sizeof(write_buf));
		//sprintf(write_buf, "%s %d\n", "hello world", iCount++);

		int len_buff = strlen(write_buf);
		int len_send = write(client_fd, write_buf, len_buff);
		if (len_buff != len_send)
		{
			printf("send buf error\n");
		}

		printf("%s len_buff:%d,len_send:%d,buf:%s\n", getStrTime(), len_buff, len_send, write_buf);

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
			//printf("recv - len:%d,buf:%s.\n", strlen(read_buf), read_buf);
		}
		sleep(1);
	}


}



