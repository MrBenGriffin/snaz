//
// Created by Ben on 2019-02-06.
//

#include "Regex.h"

#include <string>
#include <pcre.h>
#include <dlfcn.h>

//#include "commons/environment/environment.h"
#include "Regex.h"
#include "Convert.h"
#include "Dynamic.h"

using namespace std;

namespace Support {

	bool Regex::loadattempted = false;
	bool Regex::loaded = false;
	void* Regex::pcre_lib_handle = nullptr;

	int Regex::re_options = ( PCRE_UTF8 | PCRE_NO_UTF8_CHECK ) & 0x1FFFFFF; // PCRE_EXTENDED will see spaces as comments.
	int Regex::mt_options = 0 & 0x1FFFFFF;
	pcre* (*Regex::pcre_compile)(const char*, int, const char**, int*, const unsigned char*) = nullptr;
	int (*Regex::pcre_exec)(const pcre*,const pcre_extra*,PCRE_SPTR,int,int,int,int*,int) = nullptr;
	int (*Regex::pcre_config)(int,void *)= nullptr;
	//	void (*Regex::pcre_free)(void *) = nullptr;


	bool Regex::available(Messages& e) {
		if (!loadattempted) startup(e);
		return loaded;
	}

	bool Regex::startup(Messages& e) {
		bool err = false; //necessary IFF script uses pcre.
		if ( ! loadattempted ) {
			loadattempted = true;
			loaded = false;
			string pcrelib = SO(libpcre); //directory
			pcre_lib_handle = dlopen(pcrelib.c_str(),RTLD_GLOBAL | RTLD_NOW);
			err = dlerr(e);
			if (!err && pcre_lib_handle != nullptr ) {
				pcre_compile = (pcre* (*)(const char*, int, const char**, int*,const unsigned char*)) dlsym(pcre_lib_handle,"pcre_compile"); err = dlerr(e);
				if (!err) {
					pcre_exec = (int (*)(const pcre*,const pcre_extra*,PCRE_SPTR,int,int,int,int*,int)) dlsym(pcre_lib_handle,"pcre_exec"); err = dlerr(e);
				}
				if (!err) {
					pcre_config = (int (*)(int,void *)) dlsym(pcre_lib_handle,"pcre_config");  err = dlerr(e);
				}
				if ( !err && pcre_config != nullptr && pcre_compile != nullptr && pcre_exec!=nullptr) {
					int locr = 0;
					int dobo = pcre_config(PCRE_CONFIG_UTF8, &locr);
					if (locr != 1 || dobo != 0) { //dobo means that the flag is not supported...
						e << Message(debug,"Regex::startup() The pcre library was loaded, but utf-8 appears not to be supported.");
					}
					loaded = true;
				}
			}
		}
		return loaded;
	}

	bool Regex::shutdown() {											 //necessary IFF script uses pcre.
		if ( pcre_lib_handle != nullptr ) {
			dlclose(pcre_lib_handle);
//			pcre_free(pcre_lib_handle);
		}
		return true;
	}

	//• --------------------------------------------------------------------------
	//•	Return the number of replaces that took place.
	//•	eg find "^foo([a-z]+)" against "foobar woobar" using substitute "\1 is bar" ==> "bar is bar woobar"
	int Regex::replace(Messages& e,const string &pattern,const string &substitute,string &scope) {
		int count = 0;
		if ( ! scope.empty() ) {
			int* ov = nullptr;		// size must be a multiple of 3.
			int ovc = 90;   	// size of ovector
			ov = new int[ovc];
			size_t lastend = string::npos;
			string basis = scope;
			size_t start = 0;
			size_t base_len = basis.length();
			scope.clear();

			while (start <= base_len) {
				int matches = matcher(e, pattern, basis, (int)start, ov, ovc);
				if (matches <= 0) {
					if (start == 0) scope=basis; //nothing changed at all.
					break;	//we are finished - either with nothing, or all done.
				}
				size_t matchstart = ov[0], matchend = ov[1];
				if (matchstart == matchend && matchstart == lastend) {
					matchend = start + 1;
					if (start+1 < base_len && basis[start] == '\r' && basis[start+1] == '\n' ) {
						matchend++;
					}
					while (matchend < base_len && (basis[matchend] & 0xc0) == 0x80) {
						matchend++;
					}
					if (matchend <= base_len) {
						scope.append(basis, start, matchend - start);
					}
					start = matchend;
				} else {
					scope.append(basis, start, matchstart - start);
					size_t o = 0, s = 0 ;
					do {
						s = substitute.find('\\',o);
						scope.append(substitute.substr(o,s-o));
						if ( s != string::npos) {
							o = s+2;
							int c = substitute[s+1];
							if ( isdigit(c) ) {
								int n = (c - '0');
								int st = ov[2 * n];
								if (st >= 0) {
									scope.append(basis.c_str() + st, ov[2 * n + 1] - st);
								}
							} else if (c == '\\') {
								scope.push_back('\\');
							}
						}
					} while ( s != string::npos );
					start = matchend;
					lastend = matchend;
					count++;
				}
				count++;
			}
			if (count != 0 && start < base_len) {
				scope.append(basis, start, base_len - start);
			}
			delete [] ov;
		}
		return count;
	}

	bool Regex::match(Messages& e,const string &pattern, const string &scope) {         //match substring using pcre
		bool retval = false;
		if ( pattern.empty() ) {
			retval = true;
		} else {
			int* ov;	// size must be a multiple of 3.
			int ovc = 90;  // size of ovector
			ov = new int[ovc];
			int result = matcher(e, pattern, scope, 0, ov, ovc);
			retval = result > 0;
			delete [] ov;
		}
		return retval;
	}

	bool Regex::fullMatch(Messages& e,const string &pattern, const string &scope) {     //match entire string using pcre
		bool retval = false;
		if ( pattern.empty() ) {
			retval = scope.empty();
		} else {
			int* ov;		// size must be a multiple of 3.
			int ovc = 90;  // size of ovector
			ov = new int[ovc];
			int result = matcher(e, pattern, scope, 0, ov, ovc);
			if (result > 0 ) {  //at least one match was found.
				int matchstart = ov[0];
				int matchsize = ov[1];
				if (matchstart == 0 && matchsize == scope.length() ) {
					retval = true;
				}
			}
			delete [] ov;
		}
		return retval;
	}

	//• Regex private methods compile(), matcher(), reporterror() ------
	int Regex::matcher(Messages& e,const string& pattern,const string& scope,const int offset,int*& ov,int& ovc) {
		int result = 0;
		pcre *re = nullptr;
		if ( compile(e, pattern,re) ) {
			result = pcre_exec(re,nullptr,scope.c_str(),(int)scope.length(),offset,mt_options,ov,ovc);
			if ( result < 1 ) {
				if (result != -1) reportError(e,result);
				result = 0;
			}
			free(re);
		}
		return result;
	}

	bool Regex::compile(Messages& e,const string& pattern,pcre*& container) {
		const char *bad = nullptr;
		int errOffset=0;
		bool retval = true;
		container = pcre_compile(pattern.c_str(),re_options ,&bad,&errOffset,nullptr);
		if ( container == nullptr) {
			string errmsg;
			if (bad != nullptr) {
				errmsg = bad;
			} else {
				errmsg = "Unspecified compile error";
			}
			ostringstream errs;
			errs << "Regex::compile_re() " << errmsg;
			if (errOffset > 0) errs << " at offset " << errOffset;
			errs << " while compiling pattern '" << pattern << "'.";
			e << Message(error,errs.str());
			retval = false;
		}
		return retval;
	}

	void Regex::reportError(Messages& e,int errnum) {
		string msg;
		switch(errnum) {
			case  0: msg="Internal Error: ovector is too small"; break;
			case -2: msg="Internal Error: Either 'code' or 'subject' was passed as nullptr, or ovector was nullptr and ovecsize was not zero."; break;
			case -3: msg="Internal Error: An unrecognized bit was set in the options argument."; break;
			case -4: msg="Internal Error: Endian test magic number was missing. Probably a junk pointer was passed."; break;
			case -5: msg="Internal Error: An unknown item in the compiled pattern was encountered during match."; break;
			case -6: msg="Internal Error: Internal library malloc failed."; break;
			case -8: msg="The backtracking limit (as specified by the default match_limit field) was reached."; break;
			case -10: msg="A domain that contains an invalid UTF-8 byte sequence was passed."; break;
			case -11: msg="Internal Error: The UTF-8 domain was valid, but the value of startoffset did not point to the beginning of a UTF-8 character."; break;
			case -12: msg="The domain did not match, but it did match partially"; break;
			case -13: msg="Internal Error: The PARTIAL option was used with a pattern containing items that are not supported for partial matching."; break;
			case -14: msg="Internal Error: An unexpected internal error has occurred."; break;
			case -15: msg="Internal Error: The value of the ovecsize argument was negative."; break;
			case -21: msg="Recursion Limit reached. The internal recursion vector of size 1000 was not enough"; break;
			default: msg="Unknown Error"; break;
		}
		ostringstream errs;
		errs << "Regex::match() error. Error " << errnum << ". " << msg;
		e << Message(error,errs.str());
	}

	//• --------------------------------------------------------------------------
}
