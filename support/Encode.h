//
// Created by Ben Griffin on 2019-01-30.
//

#ifndef MACROTEXT_ENCODE_H
#define MACROTEXT_ENCODE_H

#include <string>
#include "support/Message.h"

namespace Support {
	using namespace std;
	bool normalise(string&); //utf-8 normalise

	void fileEncode(string&);

//	size_t calcEntity(const char*&,const char*);
	void xmldecode(string&);  //XML de-escape
	void xmlencode(string&);  //XML escape
	bool fileencode(string&); //make sure it's a legal xml name

	string hexencode(const unsigned char);

	bool tohex(string&);
	bool fromhex(Messages&,string&);

	void urlencode(string&);
	void urldecode(Messages&,string&);

	bool base64encode(string&,bool = true);
	bool base64decode(Messages&,string&);

	void tolower(string&);							//Given an mixed case string, return it in lower case. e.g. "ThIs" => "this"
	void toupper(string&);							//Given an mixed case string, return it in lower case. e.g. "ThIs" => "this"

};


#endif //MACROTEXT_ENCODE_H
