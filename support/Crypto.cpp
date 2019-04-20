//
// Created by Ben on 2019-04-17.
//


#include <cstring>
#include <dlfcn.h>

#include "support/Encode.h"
#include "support/Crypto.h"
#include "support/Dynamic.h"

namespace Support {
	//--------------------------------------------- DIGEST ------------------------------------
	unsigned long long Digest::hash(const string&  to_hash) {
		unsigned long long hashing = 5381; //DJBHash
		for(std::size_t i = 0; i < to_hash.length(); i++) {
			hashing = ((hashing << 5) + hashing) + to_hash[i];
		}
		return hashing;
	}

	string Digest::hash(string to_hash,Messages& errs) {
		if (available(errs) ) {
			string hashed = to_hash;
			do_digest(Digest::md5,hashed);
			tohex(hashed);
			return hashed;
		} else {
			unsigned long long hashnum = hash(to_hash); //DJBHash
			return to_string(hashnum);
		}
	}

	//--------------------------------------------- DIGEST LIBRARY ------------------------------
	bool Digest::loadattempted = false;
	bool Digest::loaded = false;
	void* Digest::lib_handle = nullptr;
	void* Digest::crp_handle = nullptr;
	EVP_MD_CTX* Digest::context = nullptr;
	const EVP_MD* Digest::md[16] = {nullptr,nullptr,nullptr,nullptr,nullptr, nullptr,nullptr,nullptr,nullptr,nullptr, nullptr,nullptr,nullptr,nullptr,nullptr, nullptr};
	int (*Digest::OPENSSL_init_crypto)(uint64_t, void*) = nullptr;
	EVP_MD_CTX* (*Digest::EVP_MD_CTX_new)() = nullptr;
	void (*Digest::EVP_MD_CTX_free)(EVP_MD_CTX*) = nullptr;
	const EVP_MD* (*Digest::EVP_get_digestbyname)(const char*) = nullptr;
	int (*Digest::EVP_DigestInit_ex)(EVP_MD_CTX*, const EVP_MD*,ENGINE*) = nullptr;
	int (*Digest::EVP_DigestUpdate)(EVP_MD_CTX*, const void *,size_t) = nullptr;
	int (*Digest::EVP_DigestFinal_ex)(EVP_MD_CTX*,unsigned char *,unsigned int*) = nullptr;
	int (*Digest::RAND_bytes)(unsigned char*,int) = nullptr;
	int (*Digest::RAND_pseudo_bytes)(unsigned char*,int) = nullptr;

	bool Digest::available(Messages& errors) {
		if (!loadattempted) {
			startup(errors);
		}
		return loaded;
	}
	bool Digest::startup(Messages& errors) {
#ifdef __MACH__
//#define SSLPATH "/usr/local/opt/openssl@1.1/lib/"
#else
//#define SSLPATH ""
#endif

		if ( ! loadattempted ) {
			loadattempted = true;
			loaded = false;
			string libstr = ""; libstr.append(SO(libssl)); //directory
			string crpstr = ""; crpstr.append(SO(libcrypto)); //directory
			lib_handle = dlopen(libstr.c_str(),RTLD_GLOBAL | RTLD_NOW); dlerr(errors);
			crp_handle = dlopen(crpstr.c_str(),RTLD_GLOBAL | RTLD_NOW); dlerr(errors);
			if (!errors.marked() && lib_handle != nullptr && crp_handle != nullptr) {
				OPENSSL_init_crypto	=(int (*)(uint64_t, void*)) dlsym(crp_handle,"OPENSSL_init_crypto"); dlerr(errors);
				EVP_MD_CTX_new			=(EVP_MD_CTX* (*)()) dlsym(crp_handle,"EVP_MD_CTX_new"); dlerr(errors);
				EVP_MD_CTX_free			=(void (*)(EVP_MD_CTX*)) dlsym(crp_handle,"EVP_MD_CTX_free"); dlerr(errors);
				EVP_get_digestbyname	=(const EVP_MD* (*)(const char*)) dlsym(crp_handle,"EVP_get_digestbyname"); dlerr(errors);
				EVP_DigestInit_ex		=(int (*)(EVP_MD_CTX*,const EVP_MD*,ENGINE*)) dlsym(crp_handle,"EVP_DigestInit_ex"); dlerr(errors);
				EVP_DigestUpdate		=(int (*)(EVP_MD_CTX*,const void*,size_t)) dlsym(crp_handle,"EVP_DigestUpdate"); dlerr(errors);
				EVP_DigestFinal_ex		=(int (*)(EVP_MD_CTX*,unsigned char*,unsigned int*)) dlsym(crp_handle,"EVP_DigestFinal_ex"); dlerr(errors);
				RAND_bytes				=(int (*)(unsigned char*,int)) dlsym(crp_handle,"RAND_bytes"); dlerr(errors);
				RAND_pseudo_bytes		=(int (*)(unsigned char*,int)) dlsym(crp_handle,"RAND_pseudo_bytes"); dlerr(errors);
				if (!errors.marked()) {
					loaded = true;
					OPENSSL_init_crypto(0x00000008L, nullptr);
					context = EVP_MD_CTX_create();
					md[0] = EVP_get_digestbyname("md2");
					md[1] = EVP_get_digestbyname("md4");
					md[2] = EVP_get_digestbyname("md5");
					md[3] = EVP_get_digestbyname("sha");
					md[4] = EVP_get_digestbyname("sha1");
					md[5] = EVP_get_digestbyname("sha224");
					md[6] = EVP_get_digestbyname("sha256");
					md[7] = EVP_get_digestbyname("sha384");
					md[8] = EVP_get_digestbyname("sha512");
					md[9] = EVP_get_digestbyname("mdc2");
					md[10] = EVP_get_digestbyname("ripemd160");

					auto *ibuff = new unsigned char[16];
					RAND_pseudo_bytes(ibuff,16); //err = 1 on SUCCESS.
					unsigned int seed = *(unsigned int *)ibuff;
					srand((unsigned)time(nullptr)+seed);
					delete [] ibuff;

				}
			}
		}
		return loaded;
	}
	bool Digest::shutdown() {											 //necessary IFF script uses pcre.
		if (loaded) {
			EVP_MD_CTX_destroy(context);
		}
		if ( lib_handle != nullptr ) {
			dlclose(lib_handle);
		}
		if ( crp_handle != nullptr ) {
			dlclose(crp_handle);
		}
		return true;
	}
	void Digest::do_digest(const digest d,string& basis) {
		if(loaded) {
			unsigned char md_value[EVP_MAX_MD_SIZE];
			unsigned int md_len;
			const EVP_MD *d_touse = nullptr;
			switch (d) {
				case md2: { d_touse=md[0];} break;
				case md4: { d_touse=md[1];} break;
				case md5: { d_touse=md[2];} break;
				case sha: { d_touse=md[3];} break;
				case sha1: { d_touse=md[4];} break;
				case sha224: { d_touse=md[5];} break;
				case sha256: { d_touse=md[6];} break;
				case sha384: { d_touse=md[7];} break;
				case sha512: { d_touse=md[8];} break;
				case mdc2: { d_touse=md[9];} break;
				case ripemd160: { d_touse=md[10];} break;
			}
			if (d_touse != nullptr) {
				EVP_DigestInit_ex(context, d_touse, nullptr);
				EVP_DigestUpdate(context,basis.c_str(),basis.size());
				EVP_DigestFinal_ex(context, md_value, &md_len);
				basis = string((char *)md_value,md_len);
			} else {
				basis = "Digest for that format was not found";
				//encoder was not present.
			}
		} else {
			basis = "Digest Library not loaded";
		}
	}

	void Digest::random(string& result,unsigned short size) {
		for (size_t i=0; i < size; i++) {
			result.push_back((unsigned char)(rand() & 0xFF ));
		}
		//The following seems to weigh slightly to the lower side. not sure why.
//		unsigned char *ibuff = new unsigned char[size];
//		RAND_pseudo_bytes(ibuff,size); //err = 1 on SUCCESS.
//		result = string((const char *)ibuff,0,size);
//		delete ibuff;
	}

//--------------------------------------------- DEFLATE ------------------------------------
	string Deflate::libversion;
	int Deflate::level = Z_DEFAULT_COMPRESSION;
	int Deflate::zsize=(int)sizeof(z_stream);
	bool Deflate::loadattempted = false;
	bool Deflate::loaded = false;

	void* Deflate::lib_handle = nullptr;
//loaded methods
	const char* (*Deflate::lib_zlibVersion)() = nullptr;
	int (*Deflate::lib_deflateInit)(z_streamp,int,const char*,int) = nullptr;
	int (*Deflate::lib_deflate)(z_streamp,int) = nullptr;
	int (*Deflate::lib_deflateEnd)(z_streamp) = nullptr;
	int (*Deflate::lib_inflateInit)(z_streamp,const char*,int) = nullptr;
	int (*Deflate::lib_inflate)(z_streamp,int) = nullptr;
	int (*Deflate::lib_inflateEnd)(z_streamp) = nullptr;
	uLong (*Deflate::lib_deflateBound)(z_streamp,uLong) = nullptr;

	bool Deflate::available(Messages& errors) {
		if (!loadattempted) {
			startup(errors);
		}
		return loaded;
	}
	bool Deflate::startup(Messages& errors) {
		if ( ! loadattempted ) {
			loadattempted = true;
			loaded = false;
#ifdef __MACH__
			string libstr = "libz.dylib";
#else
			string libstr = "libz.so";
#endif
			lib_handle = dlopen(libstr.c_str(),RTLD_GLOBAL | RTLD_NOW); dlerr(errors);
			if (!errors.marked()&& lib_handle != nullptr ) {
				//load dl functions .
				lib_zlibVersion  =(const char* (*)()) dlsym(lib_handle,"zlibVersion"); dlerr(errors);
				lib_deflateInit =(int (*)(z_streamp,int,const char*,int)) dlsym(lib_handle,"deflateInit_"); dlerr(errors);
				lib_deflate =(int (*)(z_streamp,int)) dlsym(lib_handle,"deflate"); dlerr(errors);
				lib_deflateEnd =(int (*)(z_streamp)) dlsym(lib_handle,"deflateEnd"); dlerr(errors);
				lib_inflateInit =(int (*)(z_streamp,const char*,int)) dlsym(lib_handle,"inflateInit_"); dlerr(errors);
				lib_inflate =(int (*)(z_streamp,int)) dlsym(lib_handle,"inflate"); dlerr(errors);
				lib_inflateEnd =(int (*)(z_streamp)) dlsym(lib_handle,"inflateEnd"); dlerr(errors);
				lib_deflateBound = (uLong (*)(z_streamp,uLong)) dlsym(lib_handle,"deflateBound"); dlerr(errors);

				if (!errors.marked() ) {
					loaded = true;
					const char* zlv = lib_zlibVersion();
					if (zlv != nullptr) { libversion = zlv; }
				}
			}
		}
		return loaded;
	}
	bool Deflate::shutdown() {											 //necessary IFF script uses pcre.
		if (loaded) {
// do any teardown functions here.
		}
		loaded=false;
		loadattempted = false;
		if ( lib_handle != nullptr ) {
			dlclose(lib_handle);
			lib_handle=nullptr;
		}
		return true;
	}
	bool Deflate::noerrs(int err,int expected,Messages& errstr) {
		bool retval = true;
		if (err != expected) {
			retval = false;
			switch (err) {
				case Z_ERRNO: {
					errstr << Message(error,"read/write error");
				} break;
				case Z_STREAM_ERROR: {
					errstr << Message(error,"invalid compression level");
				} break;
				case Z_DATA_ERROR: {
					errstr << Message(error,"invalid or incomplete deflate data");
				} break;
				case Z_MEM_ERROR: {
					errstr << Message(error,"out of memory");
				} break;
				case Z_VERSION_ERROR: {
					errstr << Message(error,"zlib version mismatch");
				} break;
				default: {
					retval = true;
				} break;
			}
		}
		return retval;
	}

	void Deflate::deflate(string& context,Messages& errstr) {
		if (!context.empty()) {
			auto bsize = (uLong)context.size();
			z_stream strm;
			strm.zalloc = nullptr;
			strm.zfree  = nullptr;
			strm.opaque = nullptr;
			auto *ibuff = new Byte[bsize];
			memcpy((char *)ibuff,context.c_str(),bsize);
			context.clear();
			int err = Z_OK;
			if (noerrs(lib_deflateInit(&strm,level,libversion.c_str(),zsize),Z_OK,errstr)) {
				uLong destiny = lib_deflateBound(&strm,bsize);
				auto *obuff = new Byte[destiny];
				strm.next_out = obuff;
				strm.next_in = ibuff;
				strm.avail_in =  (uInt)bsize;
				strm.avail_out = (uInt)destiny; //this is what we are deflating
				do {
					err = lib_deflate(&strm,Z_FINISH);
					if (noerrs(err,Z_OK,errstr)) {
						if (obuff < strm.next_out ) {
							context.append(string(obuff,strm.next_out - 1));
						} else {
							err = Z_STREAM_END;
						}
						strm.avail_out = (uInt)destiny; //this is what we are deflating
						strm.next_out = obuff;
					}
				} while (err != Z_STREAM_END );
				if (noerrs(lib_deflateEnd(&strm),Z_OK,errstr)) {
					if (obuff < strm.next_out ) {
						context.append(string(obuff,strm.next_out - 1));
					}
				}
				delete [] obuff;
			}
			delete [] ibuff;
		}
	}

	void Deflate::inflate(string& context,Messages& errstr) {
		if (!context.empty()) {
			z_stream strm;
			auto bsize = (uLong)context.size();
			strm.zalloc = nullptr;
			strm.zfree  = nullptr;
			strm.opaque = nullptr;
			auto *ibuff = new Byte[context.size()];
			memcpy((char *)ibuff,context.c_str(),bsize);
			context.clear();
			int err = Z_OK;
			if (noerrs(lib_inflateInit(&strm,libversion.c_str(),zsize),Z_OK,errstr)) {
				uLong destiny = lib_deflateBound(&strm,bsize);
				auto *obuff = new Byte[destiny];
				strm.next_out = obuff;
				strm.next_in = ibuff;
				strm.avail_in =  (uInt)bsize;
				strm.avail_out = (uInt)destiny; //this is what we are deflating
				do {
					err = lib_inflate(&strm,Z_FINISH);
					if (noerrs(err,Z_OK,errstr)) {
						if (obuff < strm.next_out ) {
							context.append(string(obuff,strm.next_out));
						} else {
							err = Z_STREAM_END;
						}
						strm.avail_out = (uInt)destiny; //this is what we are deflating
						strm.next_out = obuff;
					}
				} while (err != Z_STREAM_END );
				if (noerrs(lib_inflateEnd(&strm),Z_OK,errstr)) {
					if (obuff < strm.next_out ) {
						context.append(string(obuff,strm.next_out));
					}
				}
				delete [] obuff;
			}
			delete [] ibuff;
		}
	}
}
