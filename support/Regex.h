//
// Created by Ben on 2019-02-06.
//

#ifndef MACROTEXT_REGEX_H
#define MACROTEXT_REGEX_H


#include <string>
#include <deque>
#include <map>
#include <pcre.h>

#include "Message.h"

using namespace std;

extern "C" {
typedef struct real_pcre pcre;
}

namespace Support {
	using RegexResult=std::deque<pair<pair<int,int>,string>>;
	class Regex {
	private:
		static void * pcre_lib_handle;
		static bool loadattempted;	//used to show if the service is up or down.
		static bool loaded;	//used to show if the service is up or down.

		//The pcre object(s) that we use.
		static int re_options;
		static int mt_options;
		static bool compile(Messages&,const string &, pcre*& );
		static void reportError(Messages&,int);
//		static const char* dlerr(Messages& e);

		//The pcre API that we use.
		static pcre* (*pcre_compile)(const char*, int, const char**, int*,const unsigned char*);
		static int (*pcre_exec)(const pcre*,const pcre_extra*,PCRE_SPTR,int,int,int,int*,int);
		static int (*pcre_config)(int,void *);
		//static void (*pcre_free)(void *); // appears to be causing crashes.

	public:
		static bool startup(Messages&);
		static bool available(Messages& e);
		static bool shutdown();

		static int matcher(Messages&,const string&, const string&, int,int*&,int&);

//		static RegexResult find(Messages&,const string &,const string &);  			 //.
		static int	replace(Messages&,const string &,const string &,string &);   //replace uses pcre - bbedit like search/replace stuff.
		static bool match(Messages&,const string &,const string &);              //match substring using pcre
		static bool fullMatch(Messages&,const string &,const string &);          //match entire string using pcre
	};
}


#endif //MACROTEXT_REGEX_H
