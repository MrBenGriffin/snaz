//
// Created by Ben Griffin on 2019-01-30.
//

#ifndef MACROTEXT_CONVERT_H
#define MACROTEXT_CONVERT_H

#include <string>
#include <vector>
#include <deque>
#include <set>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>

namespace Support {
	using namespace std;

	void tabstrip(string&);
	void wsstrip(string&);
	void strip(string&);
	void trim(string&);
	void wtrim(string&);

	string tostring(unsigned long long);
	string tofixedstring(int,int);
	string tostring(string::const_iterator&, char);	//takes string
	string tostring(int);
	string tostring(size_t);
	string tostring(double,size_t=0);
	string tostring(long double,string);
	void tostring(string&,int);			//Given an integer, return a string. e.g. 3 => "3"
	void tostring(string&,size_t);		//Given a natural, return a string. e.g. 3 => "3"
	pair<string,string> split(char, const string&);	   //Given a string, split it at the first given character (destroy the character)
	bool split(char, const string&, pair<string,string>&);	   //Given a string, split it at the first given character (destroy the character)
	bool rsplit(char, const string&, pair<string,string>&);	   //Given a string, split it at the last given character (destroy the character)

	pair<size_t,bool> znatural(const string&,size_t&);
	pair<size_t,bool> znatural(const string&);			//Given a string, returns a natural  0... +BIGINT and a flag saying if it is a number at all.
	pair<size_t,bool> znatural(string::const_iterator&); //Given a string, returns a natural  0... +BIGINT and a flag saying if it is a number at all.
	pair<size_t,bool> znatural(string::const_iterator&,string::const_iterator&); //Given a string, returns a natural  0... +BIGINT and a flag saying if it is a number at all.

	void			todigits(string&);							//strip all but digits from a string.
	size_t	natural(const string&);						//Given a const string, returns a natural  1... +BIGINT
	size_t	natural(string::const_iterator&);			//Given a const string, returns a natural  1... +BIGINT
	pair<std::time_t,bool> time_t(const string&);
	pair<long,bool>  integer(const string&);						//Given a string, returns an integer -BIGINT ... 0 ... +BIGINT and a flag saying if it is a number at all.
	long			integer(string::const_iterator&);			//Given a const string, returns an integer -BIGINT ... 0 ... +BIGINT
	double			real(string::const_iterator&);				//Given a string, returns a double.
	double			real(const string&);						//Given a const string, returns a double (floating point).
	size_t	hex(string::const_iterator&,double&); 		//input e.g. x32dda outputs a hex double. error needs NaN returns bytes used
	void			tolist(deque<size_t>&,const string &); //given a comma delimited set of naturals, returns a deque of naturals
	void			tolist(vector<size_t>&,const string &); //given a comma delimited set of naturals, returns a vector of naturals
	void 			tolist(vector<string>&,string,const string&); //given a cutter(string) delimited set of strings, return a vector of strings.
	void 			toSet(set<size_t>&,const string&); //given a comma-delimited set of strings, return a set of size_t.

	void			tolower(string&);							//Given an mixed case string, return it in lower case. e.g. "ThIs" => "this"
	void			toupper(string&);							//Given an mixed case string, return it in lower case. e.g. "ThIs" => "this"
	bool			isint(const string&);
	bool			isfloat(const string &);
	bool			isdouble(const string &);
	string			fixUriSpaces(const string& uriString);

	//						set<size_t> languages;

};


#endif //MACROTEXT_CONVERT_H
