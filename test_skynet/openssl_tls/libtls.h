#ifndef __TLS_H__
#define __TLS_H__

#include <stddef.h>

#define init_bio _init_bio
#define init_client_context _init_client_context
#define init_server_context _init_server_context

#define check_context _check_context
#define check_sslctx _check_sslctx

#define bio_read _bio_read
#define bio_write _bio_write

#define tls_context_finished _ltls_context_finished
#define tls_context_close _ltls_context_close
#define tls_context_handshake _ltls_context_handshake
#define tls_context_read _ltls_context_read
#define tls_context_write _ltls_context_write

#define ctx_gc _lctx_gc
#define ctx_cert _lctx_cert
#define ctx_ciphers _lctx_ciphers

#define new_ctx lnew_ctx
#define new_tls lnew_tls

struct tls_context;
struct ssl_ctx;

void _init_bio(struct tls_context* tls_p, struct ssl_ctx* ctx_p);
void _init_client_context(struct tls_context* tls_p, struct ssl_ctx* ctx_p);
void _init_server_context(struct tls_context* tls_p, struct ssl_ctx* ctx_p);

struct tls_context * _check_context(void * parm);
struct ssl_ctx * _check_sslctx(void * parm);

int _bio_read(struct tls_context* tls_p, char ** out_read);
void _bio_write(struct tls_context* tls_p, const char* s, size_t len);

int _ltls_context_finished(struct tls_context* tls_p);
int _ltls_context_close(struct tls_context* tls_p);
int _ltls_context_handshake(struct tls_context* tls_parm, size_t slen, const char* exchange, char** out_read);
int _ltls_context_read(struct tls_context* tls_parm, size_t slen, const char* encrypted_data);
int _ltls_context_write(struct tls_context* tls_parm, size_t slen, char* unencrypted_data);

int _lctx_gc(struct ssl_ctx* ctx_parm);
int _lctx_cert(struct ssl_ctx* ctx_parm, const char* certfile, const char* key);
int _lctx_ciphers(struct ssl_ctx* ctx_parm, const char* ciphers);

struct ssl_ctx* lnew_ctx();
struct tls_context* lnew_tls(const char* method_parm, struct ssl_ctx* ctx_parm);

#endif
