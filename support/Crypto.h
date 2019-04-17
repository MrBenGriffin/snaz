//
// Created by Ben on 2019-04-17.
//

#ifndef MACROTEXT_CRYPTO_H
#define MACROTEXT_CRYPTO_H

#include <string>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <zlib.h>
#include "support/Message.h"

namespace Support {

	class Deflate {
	private:
		static void* lib_handle;
		static bool loadattempted;	//used to show if the service is up or down.
		static bool loaded;			//used to show if the service is up or down.
		static const char* (*lib_zlibVersion)();
		static int (*lib_deflateInit)(z_streamp,int,const char*,int);
		static int (*lib_deflate)(z_streamp,int);
		static int (*lib_deflateEnd)(z_streamp);
		static int (*lib_inflateInit)(z_streamp,const char*,int);
		static int (*lib_inflate)(z_streamp,int);
		static int (*lib_inflateEnd)(z_streamp);
		static uLong (*lib_deflateBound)(z_streamp,uLong);

		//specific private functions
		static string libversion;
		static int zsize;
		static int level;
		static bool noerrs(int,int,Messages&);

	public:
		static bool startup(Messages&);
		static bool available(Messages&);
		static bool shutdown();
		static void deflate(string&,Messages&);
		static void inflate(string&,Messages&);

	};

	class Digest {
	private:
		static void* lib_handle;
		static void* crp_handle;
		static bool loadattempted;	//used to show if the service is up or down.
		static bool loaded;			//used to show if the service is up or down.

		static int (*OPENSSL_init_crypto)(uint64_t, void*);
		static EVP_MD_CTX* (*EVP_MD_CTX_create)();
		static void (*EVP_MD_CTX_destroy)(EVP_MD_CTX*);
		static const EVP_MD* (*EVP_get_digestbyname)(const char*);
		static int (*EVP_DigestInit_ex)(EVP_MD_CTX*, const EVP_MD*,ENGINE*);
		static int (*EVP_DigestUpdate)(EVP_MD_CTX*, const void *,size_t);
		static int (*EVP_DigestFinal_ex)(EVP_MD_CTX*,unsigned char *,unsigned int*);

		static int (*RAND_bytes)(unsigned char*,int);
		static int (*RAND_pseudo_bytes)(unsigned char*,int);

		static EVP_MD_CTX* context;
		static const EVP_MD* md[16];	//the different digests

	public:

		typedef enum {md2,md4,md5,sha,sha1,sha224,sha256,sha384,sha512,mdc2,ripemd160} digest;	//cookie_expiry,cookie_path,cookie_domain -- cannot be retrieved from server..
		static bool startup(Messages&);
		static bool available(Messages&);
		static bool shutdown();
		static void do_digest(digest,string&);
		static void random(string&,unsigned short);

		//Do not need external library..
		static unsigned long long hash(const string&);
		static string hash(string,Messages&);

	};
}

#endif //MACROTEXT_CRYPTO_H
