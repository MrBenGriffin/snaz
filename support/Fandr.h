//
// Created by Ben Griffin on 2019-01-30.
//

#ifndef MACROTEXT_FANDR_H
#define MACROTEXT_FANDR_H
#include <string>
#include <vector>


namespace Support {
using namespace std;

unsigned const int MAX_MATCH = 9;		//maximum regexp matches

//	bool fandr(string&,const vector<pair<string,string>>&,size_t=0);
int fandr(string&, string);				    //find and replace with emptystring
int fandr(string&, string, size_t i);  //find and replace with integer
int fandr(string&, string, string);    //find and replace with substitute string
bool length(string const,size_t&);
bool char_at(string const,size_t,string&);
bool position(string const,string const,size_t&);	//return the character position of first string in second string.
size_t char_col(string const,size_t,size_t);			//given a row and column (counted in bytes), return the character column value
size_t char_col(string const,size_t);							//given a column (counted in bytes), return the character column value
bool left(string const,long,string&);
bool right(string const,long,string&);
int length(char*&, int&);

}


#endif //MACROTEXT_FANDR_H
