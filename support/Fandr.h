//
// Created by Ben Griffin on 2019-01-30.
//

#ifndef MACROTEXT_FANDR_H
#define MACROTEXT_FANDR_H
#include <string>


namespace Support {
	using namespace std;

	unsigned const int MAX_MATCH = 9;		//maximum regexp matches

	int fandr(string&, string);				    //find and replace with emptystring
	int fandr(string&, string, size_t i);  //find and replace with integer
	int fandr(string&, string, string);    //find and replace with substitute string
	bool length(string const,size_t&);
	bool char_at(string const,size_t,string&);
	bool position(string const,string const,size_t&);	//return the character position of first string in second string.
	bool left(string const,long,string&);
	bool right(string const,long,string&);

}


#endif //MACROTEXT_FANDR_H
