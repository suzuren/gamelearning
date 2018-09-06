#include "skynet.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//--------------------------------------------------------------------------

char *
skynet_strdup(const char *str) {
	size_t sz = strlen(str);
	char * ret = skynet_malloc(sz + 1);
	memcpy(ret, str, sz + 1);
	return ret;
}
void
skynet_error(struct skynet_context * context, const char *msg, ...) {
	printf("skynet_error - context:%p,msg:%s\n", context, msg);
}

// logger_create return inst
// function static int logger_cb
//skynet_callback(ctx, inst, logger_cb);  
void
skynet_callback(struct skynet_context * context, void *ud, skynet_cb cb) {
	context->cb = cb;
	context->cb_ud = ud;
}

//--------------------------------------------------------------------------

uint32_t
skynet_handle_findname(const char * name) {
	uint32_t handle = 0;
	return handle;
}

const char *
skynet_handle_namehandle(uint32_t handle, const char *name) {

	const char * ret = "handle name";

	return ret;
}

// skynet command

struct command_func {
	const char *name;
	const char * (*func)(struct skynet_context * context, const char * param);
};

static const char *
cmd_timeout(struct skynet_context * context, const char * param) {
	char * session_ptr = NULL;
	int ti = strtol(param, &session_ptr, 10);
	int session = 10;// skynet_context_newsession(context);
	//skynet_timeout(context->handle, ti, session);
	sprintf(context->result, "%d", session);
	return context->result;
}

//cmd_reg(ctx, ".logger");
static const char *
cmd_reg(struct skynet_context * context, const char * param) {
	printf("cmd_reg - context:%p,param:%s\n", context, param);
	if (param == NULL || param[0] == '\0') {
		sprintf(context->result, ":%x", context->handle);
		return context->result;
	}
	else if (param[0] == '.') { // 单节点有效("."开头)
		return skynet_handle_namehandle(context->handle, param + 1);
	}
	else {
		skynet_error(context, "Can't register global name %s in C", param);
		return NULL;
	}
}

static const char *
cmd_query(struct skynet_context * context, const char * param) {
	if (param[0] == '.') {
		uint32_t handle = skynet_handle_findname(param + 1);
		if (handle) {
			sprintf(context->result, ":%x", handle);
			return context->result;
		}
	}
	return NULL;
}

static const char *
cmd_name(struct skynet_context * context, const char * param) {
	int size = strlen(param);
	char name[size + 1];
	char handle[size + 1];
	sscanf(param, "%s %s", name, handle);
	if (handle[0] != ':') {
		return NULL;
	}
	uint32_t handle_id = strtoul(handle + 1, NULL, 16);
	if (handle_id == 0) {
		return NULL;
	}
	if (name[0] == '.') {
		return skynet_handle_namehandle(handle_id, name + 1);
	}
	else {
		skynet_error(context, "Can't set global name %s in C", name);
	}
	return NULL;
}

static const char *
cmd_exit(struct skynet_context * context, const char * param) {
	//handle_exit(context, 0);
	return NULL;
}

static uint32_t
tohandle(struct skynet_context * context, const char * param) {
	uint32_t handle = 0;
	if (param[0] == ':') {
		handle = strtoul(param + 1, NULL, 16);
	}
	else if (param[0] == '.') {
		handle = skynet_handle_findname(param + 1);
	}
	else {
		skynet_error(context, "Can't convert %s to handle", param);
	}

	return handle;
}

static const char *
cmd_kill(struct skynet_context * context, const char * param) {
	uint32_t handle = tohandle(context, param);
	if (handle) {
		//handle_exit(context, handle);
	}
	return NULL;
}

static const char *
cmd_launch(struct skynet_context * context, const char * param) {
	size_t sz = strlen(param);
	char tmp[sz + 1];
	strcpy(tmp, param);
	char * args = tmp;
	char * mod = strsep(&args, " \t\r\n");
	args = strsep(&args, "\r\n");
	struct skynet_context * inst = NULL;// skynet_context_new(mod, args);
	if (inst == NULL) {
		return NULL;
	}
	else {
		//id_to_hex(context->result, inst->handle);
		return context->result;
	}
}

static const char *
cmd_getenv(struct skynet_context * context, const char * param) {
	return "logger";// skynet_getenv(param);
}

static const char *
cmd_setenv(struct skynet_context * context, const char * param) {
	size_t sz = strlen(param);
	char key[sz + 1];
	int i;
	for (i = 0; param[i] != ' ' && param[i]; i++) {
		key[i] = param[i];
	}
	if (param[i] == '\0')
		return NULL;

	key[i] = '\0';
	param += i + 1;

	//skynet_setenv(key, param);
	return NULL;
}

static const char *
cmd_starttime(struct skynet_context * context, const char * param) {
	uint32_t sec = 0;// skynet_starttime();
	sprintf(context->result, "%u", sec);
	return context->result;
}

static const char *
cmd_abort(struct skynet_context * context, const char * param) {
	//skynet_handle_retireall();
	return NULL;
}

static const char *
cmd_monitor(struct skynet_context * context, const char * param) {
	uint32_t handle = 0;
	if (param == NULL || param[0] == '\0') {
		//if (G_NODE.monitor_exit) {
		//	// return current monitor serivce
		//	sprintf(context->result, ":%x", G_NODE.monitor_exit);
		//	return context->result;
		//}
		return NULL;
	}
	else {
		//handle = tohandle(context, param);
	}
	//G_NODE.monitor_exit = handle;
	return NULL;
}

static const char *
cmd_stat(struct skynet_context * context, const char * param) {
	if (strcmp(param, "mqlen") == 0) {
		int len = 1;// skynet_mq_length(context->queue);
		sprintf(context->result, "%d", len);
	}
	else if (strcmp(param, "endless") == 0) {
		if (context->endless) {
			strcpy(context->result, "1");
			context->endless = false;
		}
		else {
			strcpy(context->result, "0");
		}
	}
	else if (strcmp(param, "cpu") == 0) {
		double t = (double)context->cpu_cost / 1000000.0;	// microsec
		sprintf(context->result, "%lf", t);
	}
	else if (strcmp(param, "time") == 0) {
		if (context->profile) {
			uint64_t ti = 6788373748494;// skynet_thread_time() - context->cpu_start;
			double t = (double)ti / 1000000.0;	// microsec
			sprintf(context->result, "%lf", t);
		}
		else {
			strcpy(context->result, "0");
		}
	}
	else if (strcmp(param, "message") == 0) {
		sprintf(context->result, "%d", context->message_count);
	}
	else {
		context->result[0] = '\0';
	}
	return context->result;
}

static const char *
cmd_logon(struct skynet_context * context, const char * param) {
	uint32_t handle = tohandle(context, param);
	if (handle == 0)
		return NULL;
	struct skynet_context * ctx = NULL;// skynet_handle_grab(handle);
	if (ctx == NULL)
		return NULL;
	FILE *f = NULL;
	FILE * lastf = ctx->logfile;
	if (lastf == NULL) {
		f = NULL;// skynet_log_open(context, handle);
		if (f) {
			//if (!ATOM_CAS_POINTER(&ctx->logfile, NULL, f)) {
			//	// logfile opens in other thread, close this one.
			//	fclose(f);
			//}
		}
	}
	//skynet_context_release(ctx);
	return NULL;
}

static const char *
cmd_logoff(struct skynet_context * context, const char * param) {
	uint32_t handle = tohandle(context, param);
	if (handle == 0)
		return NULL;
	struct skynet_context * ctx = NULL;// skynet_handle_grab(handle);
	if (ctx == NULL)
		return NULL;
	FILE * f = ctx->logfile;
	if (f) {
		// logfile may close in other thread
		//if (ATOM_CAS_POINTER(&ctx->logfile, f, NULL)) {
		//	skynet_log_close(context, f, handle);
		//}
	}
	//skynet_context_release(ctx);
	return NULL;
}

static const char *
cmd_signal(struct skynet_context * context, const char * param) {
	uint32_t handle = tohandle(context, param);
	if (handle == 0)
		return NULL;
	struct skynet_context * ctx = NULL;// skynet_handle_grab(handle);
	if (ctx == NULL)
		return NULL;
	param = strchr(param, ' ');
	int sig = 0;
	if (param) {
		sig = strtol(param, NULL, 0);
	}
	// NOTICE: the signal function should be thread safe.
	//skynet_module_instance_signal(ctx->mod, ctx->instance, sig);

	//skynet_context_release(ctx);
	return NULL;
}


static struct command_func cmd_funcs[] = {
	{ "TIMEOUT", cmd_timeout },
	{ "REG", cmd_reg },
	{ "QUERY", cmd_query },
	{ "NAME", cmd_name },
	{ "EXIT", cmd_exit },
	{ "KILL", cmd_kill },
	{ "LAUNCH", cmd_launch },
	{ "GETENV", cmd_getenv },
	{ "SETENV", cmd_setenv },
	{ "STARTTIME", cmd_starttime },
	{ "ABORT", cmd_abort },
	{ "MONITOR", cmd_monitor },
	{ "STAT", cmd_stat },
	{ "LOGON", cmd_logon },
	{ "LOGOFF", cmd_logoff },
	{ "SIGNAL", cmd_signal },
	{ NULL, NULL },
};

// skynet_command(ctx, "REG", ".logger");

const char *
skynet_command(struct skynet_context * context, const char * cmd, const char * param) {
	struct command_func * method = &cmd_funcs[0];
	while (method->name) {
		if (strcmp(cmd, method->name) == 0) {
			return method->func(context, param);
		}
		++method;
	}

	return NULL;
}

//--------------------------------------------------------------------------


struct logger {
	FILE * handle;
	char * filename;
	int close;
};

struct logger *
logger_create(void) {
	struct logger * inst = skynet_malloc(sizeof(*inst));
	inst->handle = NULL;
	inst->close = 0;
	inst->filename = NULL;

	printf("logger_create - inst:%p\n", inst);
	return inst;
}

void
logger_release(struct logger * inst) {
	printf("logger_release - filename:%s,close:%d,handle:%p\n", inst->filename, inst->close, inst->handle);

	if (inst->close) {
		fclose(inst->handle);
	}
	skynet_free(inst->filename);
	skynet_free(inst);
}

static int
logger_cb(struct skynet_context * context, void *ud, int type, int session, uint32_t source, const void * msg, size_t sz) {
	struct logger * inst = ud;
	switch (type) {
	case PTYPE_SYSTEM:
		if (inst->filename) {
			inst->handle = freopen(inst->filename, "a", inst->handle);
		}
		break;
	case PTYPE_TEXT:
		fprintf(inst->handle, "[:%08x] ",source);
		fwrite(msg, sz , 1, inst->handle);
		fprintf(inst->handle, "\n");
		fflush(inst->handle);
		break;
	}

	return 0;
}

int
logger_init(struct logger * inst, struct skynet_context *ctx, const char * parm) {

	if (parm)
	{
		inst->handle = fopen(parm,"w");
		if (inst->handle == NULL) {
			return 1;
		}
		inst->filename = skynet_malloc(strlen(parm)+1);
		strcpy(inst->filename, parm);
		inst->close = 1;
	} else {
		inst->handle = stdout;
	}
	printf("logger_init - parm:%s,filename:%s,handle:%p\n", parm, inst->filename, inst->handle);

	if (inst->handle) {
		skynet_callback(ctx, inst, logger_cb);
		skynet_command(ctx, "REG", ".logger");

		return 0;
	}
	return 1;
}
