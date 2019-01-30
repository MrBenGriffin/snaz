//
// Created by Ben Griffin on 2019-01-30.
//

#ifndef MACROTEXT_ENCODE_H
#define MACROTEXT_ENCODE_H

#include <string>

namespace Support {
	using namespace std;
	bool normalise(string&); //utf-8 normalise

	string xmlenc(string);
	string xmlenc(const char*);
	void xmldecode(string&);  //XML de-escape
	void xmlencode(string&);  //XML escape
	bool nameencode(string&); //make sure it's a legal xml name
	bool fileencode(string&); //make sure it's a legal xml name

	string hexencode(const unsigned char);

	bool tohex(string&);
	bool fromhex(string&);

	void urldecode(string&);
	void urlencode(string&);

	bool base64decode(string&);
	bool base64encode(string&,bool = true);

};


#endif //MACROTEXT_ENCODE_H
