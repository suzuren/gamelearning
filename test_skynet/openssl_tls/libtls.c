
#include "libtls.h"
#include "skynet_malloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> 
#include <openssl/err.h>
#include <openssl/dh.h>
#include <openssl/ssl.h>
#include <openssl/conf.h>
#include <openssl/engine.h>

static bool TLS_IS_INIT = false;

struct tls_context {
    SSL* ssl;
    BIO* in_bio;
    BIO* out_bio;
    bool is_server;
    bool is_close;
};

struct ssl_ctx {
    SSL_CTX* ctx;
};

void
_init_bio(struct tls_context* tls_p, struct ssl_ctx* ctx_p) {
    tls_p->ssl = SSL_new(ctx_p->ctx);
    if(!tls_p->ssl) {
		fprintf(stderr, "SSL_new faild.\n");
    }
    tls_p->in_bio = BIO_new(BIO_s_mem());
    if(!tls_p->in_bio) {
		fprintf(stderr, "new in bio faild.\n");
    }
    BIO_set_mem_eof_return(tls_p->in_bio, -1);

    tls_p->out_bio = BIO_new(BIO_s_mem());
    if(!tls_p->out_bio) {
		fprintf(stderr, "new out bio faild.\n");
    }
    BIO_set_mem_eof_return(tls_p->out_bio, -1);

    SSL_set_bio(tls_p->ssl, tls_p->in_bio, tls_p->out_bio);
}


void
_init_client_context(struct tls_context* tls_p, struct ssl_ctx* ctx_p) {
    tls_p->is_server = false;
    _init_bio(tls_p, ctx_p);
    SSL_set_connect_state(tls_p->ssl);
}

void
_init_server_context(struct tls_context* tls_p, struct ssl_ctx* ctx_p) {
    tls_p->is_server = true;
    _init_bio(tls_p, ctx_p);
    SSL_set_accept_state(tls_p->ssl);
}

struct tls_context *
	_check_context(void * parm) {
	struct tls_context* tls_p = (struct tls_context*)parm;
	if (!tls_p) {
		fprintf(stderr, "need tls context.\n");
	}
	if (tls_p->is_close) {
		fprintf(stderr, "context is closed.\n");
	}
	return tls_p;
}

struct ssl_ctx *
	_check_sslctx(void * parm) {
	struct ssl_ctx* ctx_p = (struct ssl_ctx*)parm;
	if (!ctx_p) {
		fprintf(stderr, "need sslctx.\n");
	}
	return ctx_p;
}

int
_bio_read(struct tls_context* tls_p,char ** out_read)
{
	*out_read = "";
	char outbuff[4096];
	int all_read = 0;
	int read = 0;
	int pending = BIO_ctrl_pending(tls_p->out_bio);
	if (pending >0)
	{
		//luaL_Buffer b;
		//luaL_buffinit(L, &b);
		char * preadbuff = NULL;
		size_t readsize = 0;
		while (pending > 0)
		{
			read = BIO_read(tls_p->out_bio, outbuff, sizeof(outbuff));
			if (read <= 0)
			{
				fprintf(stderr, "BIO_read error:%d.\n", read);
			}
			else if (read <= sizeof(outbuff))
			{
				all_read += read;
				//luaL_addlstring(&b, (const char*)outbuff, read);
				preadbuff = skynet_copymem(preadbuff, readsize, outbuff, read);
			}
			else
			{
				fprintf(stderr, "invalid BIO_read:%d.\n", read);
			}
			pending = BIO_ctrl_pending(tls_p->out_bio);
		}
		if (all_read>0)
		{
			//luaL_pushresult(&b);
			*out_read = preadbuff;
		}
	}
	return all_read;
}


void
_bio_write(struct tls_context* tls_p, const char* s, size_t len) {
	char* p = (char*)s;
	size_t sz = len;
	while (sz > 0) {
		int written = BIO_write(tls_p->in_bio, p, sz);
		if (written <= 0) {
			fprintf(stderr, "BIO_write error:%d.\n", written);
		}
		else if (written <= sz) {
			p += written;
			sz -= written;
		}
		else {
			fprintf(stderr, "invalid BIO_write:%d.\n", written);
		}
	}
}



int
_ltls_context_finished(struct tls_context* tls_p) {
    int b = SSL_is_init_finished(tls_p->ssl);
    return b;
}

int
_ltls_context_close(struct tls_context* tls_p) {
    if(!tls_p->is_close) {
        SSL_free(tls_p->ssl);
        tls_p->ssl = NULL;
        tls_p->in_bio = NULL;
        tls_p->out_bio = NULL;
        tls_p->is_close = true;
    }
    return 0;
}


int
_ltls_context_handshake(struct tls_context* tls_parm, size_t slen, const char* exchange, char ** out_read) {

	*out_read = NULL;
	struct tls_context* tls_p = _check_context(tls_parm);

    // check handshake is finished
    if(SSL_is_init_finished(tls_p->ssl)) {
		fprintf(stderr, "handshake is finished.\n");
    }

    // handshake exchange
    if(slen > 0 && exchange != NULL) {
        _bio_write(tls_p, exchange, slen);
    }

    // first handshake; initiated by client
    if(!SSL_is_init_finished(tls_p->ssl))
	{
        int ret = SSL_do_handshake(tls_p->ssl);
        if(ret == 1)
		{
            return 0;
        }
		else if (ret < 0)
		{
            int err = SSL_get_error(tls_p->ssl, ret);
            if(err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
			{
                int all_read = _bio_read(tls_p, out_read);
                if(all_read>0)
				{
                    return all_read;
                }
            } else {
				fprintf(stderr, "SSL_do_handshake error:%d ret:%d.\n", err, ret);
            }
        } else {
            int err = SSL_get_error(tls_p->ssl, ret);
			fprintf(stderr, "SSL_do_handshake error:%d ret:%d.\n", err, ret);
        }
    }
    return 0;
}


int
_ltls_context_read(struct tls_context* tls_parm, size_t slen, const char* encrypted_data) {
	
	struct tls_context* tls_p = _check_context(tls_parm);

    // write encrypted data
    if(slen>0 && encrypted_data) {
        _bio_write(tls_p, encrypted_data, slen);
    }

    char outbuff[4096];
    int read = 0;
    //luaL_Buffer b;
    //luaL_buffinit(L, &b);

    do {
        read = SSL_read(tls_p->ssl, outbuff, sizeof(outbuff));
        if(read <= 0) {
            int err = SSL_get_error(tls_p->ssl, read);
            if(err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
                break;
            }
			fprintf(stderr, "SSL_read error:%d.\n", err);
        }else if(read <= sizeof(outbuff)) {
            //luaL_addlstring(&b, outbuff, read);
        }else {
			fprintf(stderr, "invalid SSL_read:%d.\n", read);
        }
    }while(true);
    //luaL_pushresult(&b);
    return 1;
}


int
_ltls_context_write(struct tls_context* tls_parm, size_t slen, char* unencrypted_data) {

	struct tls_context* tls_p = _check_context(tls_parm);

    while(slen > 0) {
        int written = SSL_write(tls_p->ssl, unencrypted_data,  slen);
        if(written <= 0) {
            int err = SSL_get_error(tls_p->ssl, written);
			fprintf(stderr, "SSL_write error:%d.\n", err);
        }else if(written <= slen) {
            unencrypted_data += written;
            slen -= written;
        }else {
			fprintf(stderr, "invalid SSL_write:%d.\n", written);
        }
    }
	char * out_read;
    int all_read = _bio_read(tls_p, &out_read);
    if(all_read <= 0) {
        //lua_pushstring(L, "");
    }
    return 1;
}


int
_lctx_gc(struct ssl_ctx* ctx_parm) {
    struct ssl_ctx* ctx_p = _check_sslctx(ctx_parm);
    if(ctx_p->ctx) {
        SSL_CTX_free(ctx_p->ctx);
        ctx_p->ctx = NULL;
    }
    return 0;
}

int
_lctx_cert(struct ssl_ctx* ctx_parm, const char* certfile, const char* key) {
    struct ssl_ctx* ctx_p = _check_sslctx(ctx_parm);
    if(!certfile) {
		fprintf(stderr, "need certfile.\n");
    }

    if(!key) {
		fprintf(stderr, "need private key.\n");
    }
    int ret = SSL_CTX_use_certificate_file(ctx_p->ctx, certfile, SSL_FILETYPE_PEM);
    if(ret != 1) {
		fprintf(stderr, "SSL_CTX_use_certificate_file error:%d.\n", ret);
    }
    ret = SSL_CTX_use_PrivateKey_file(ctx_p->ctx, key, SSL_FILETYPE_PEM);
    if(ret != 1) {
		fprintf(stderr, "SSL_CTX_use_PrivateKey_file error:%d.\n", ret);
    }
    ret = SSL_CTX_check_private_key(ctx_p->ctx);
    if(ret != 1) {
		fprintf(stderr, "SSL_CTX_check_private_key error:%d.\n", ret);
    }
    return 0;
}

int
_lctx_ciphers(struct ssl_ctx* ctx_parm, const char* ciphers) {
    struct ssl_ctx* ctx_p = _check_sslctx(ctx_parm);
    if(!ciphers) {
		fprintf(stderr, "need cipher list.\n");
    }
    int ret = SSL_CTX_set_tlsext_use_srtp(ctx_p->ctx, ciphers);
    if(ret != 0) {
		fprintf(stderr, "SSL_CTX_set_tlsext_use_srtp error:%d.\n", ret);
    }
    return 0;
}


struct ssl_ctx*
lnew_ctx() {
    struct ssl_ctx* ctx_p = (struct ssl_ctx*)skynet_malloc(sizeof(*ctx_p));
    ctx_p->ctx = SSL_CTX_new(SSLv23_method());
    if(!ctx_p->ctx) {
		fprintf(stderr, "SSL_CTX_new client faild.\n");
    }
    return ctx_p;
}


struct tls_context*
lnew_tls(const char* method_parm, struct ssl_ctx* ctx_parm) {
    struct tls_context* tls_p = (struct tls_context*)skynet_malloc(sizeof(*tls_p));
    tls_p->is_close = false;
	const char* method = method_parm;
    struct ssl_ctx* ctx_p = _check_sslctx(ctx_parm);

    if(strcmp(method, "client") == 0)
	{
        _init_client_context(tls_p, ctx_p);
    }
	else if(strcmp(method, "server") == 0)
	{
        _init_server_context(tls_p, ctx_p);
    } else {
		fprintf(stderr, "invalid method:%s e.g[server, client].\n", method);
    }
    return tls_p;
}


void __attribute__((constructor)) ltls_init(void) {
#ifndef OPENSSL_EXTERNAL_INITIALIZATION
    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    TLS_IS_INIT = true;
#endif
}


void __attribute__((destructor)) ltls_destory(void) {
    if(TLS_IS_INIT) {
        ENGINE_cleanup();
        CONF_modules_unload(1);
        ERR_free_strings();
        EVP_cleanup();
        //sk_SSL_COMP_free(SSL_COMP_get_compression_methods());
        CRYPTO_cleanup_all_ex_data();
    }
}
