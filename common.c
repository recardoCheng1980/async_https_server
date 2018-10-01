#include "common.h"
#include <openssl/err.h>

BIO *bio_err=0;
static char *pass;
static int password_cb(char *buf,int num,
  int rwflag,void *userdata);
static void sigpipe_handle(int x);

/* A simple error and exit routine*/
int err_exit(char *string) {
  fprintf(stderr,"%s\n",string);
  exit(0);
}

/* Print SSL errors and exit*/
int berr_exit(char* string) {
  BIO_printf(bio_err,"%s\n",string);
  ERR_print_errors(bio_err);
  exit(0);
}

/*The password code is not thread safe*/
static int password_cb(char *buf,int num, int rwflag,void *userdata) {
  if(num<strlen(pass)+1)
    return(0);

  strcpy(buf,pass);
  return(strlen(pass));
}

static void sigpipe_handle(int x){
}

SSL_CTX *initialize_ctx() {
    SSL_METHOD *meth;
    SSL_CTX *ctx;

    if(!bio_err){
      /* Global system initialization*/
      SSL_library_init();
      SSL_load_error_strings();
      ERR_load_SSL_strings();
      ERR_load_CRYPTO_strings();
      ERR_load_crypto_strings();
      OpenSSL_add_all_algorithms();
      OpenSSL_add_all_ciphers();
      OpenSSL_add_all_digests();

      /* An error write context */
      bio_err=BIO_new_fp(stderr,BIO_NOCLOSE);
    }

    /* Set up a SIGPIPE handler */
    signal(SIGPIPE,sigpipe_handle);

    /* Create our context*/
    meth=SSLv23_method();
    ctx=SSL_CTX_new(meth);

    /* Load our keys and certificates*/
    if(!(SSL_CTX_use_certificate_file(ctx, "./server.crt", SSL_FILETYPE_PEM))) {
      berr_exit("Can't read certificate file");
    }

    if(!(SSL_CTX_use_PrivateKey_file(ctx, "./server.key", SSL_FILETYPE_PEM))) {
      berr_exit("Can't read key file");
    }

    return ctx;
}

void destroy_ctx(SSL_CTX *ctx) {
  SSL_CTX_free(ctx);
}